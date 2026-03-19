#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define MAX_DOCS 5000
#define MAX_WORDS 1000
#define MAX_VOCAB 10000
#define MAX_CLASSES 10
#define WORD_LEN 50
typedef struct {
    char word[WORD_LEN];
    int count[MAX_CLASSES];
} VocabEntry;
typedef struct {
    int label;
    char words[MAX_WORDS][WORD_LEN];
    int word_count;
} Document;
typedef struct {
    Document docs[MAX_DOCS];
    int size;
} Dataset;
typedef struct {
    VocabEntry vocab[MAX_VOCAB];
    int vocab_size;
    int class_doc_count[MAX_CLASSES];
    int total_docs;
    int total_words_in_class[MAX_CLASSES];
    double alpha;
    int num_classes;
} NaiveBayes;
typedef struct {
    int TP, FP, TN, FN;
} Metrics;
int find_word(NaiveBayes* nb, const char* word) {
    for (int i = 0; i < nb->vocab_size; i++) {
        if (strcmp(nb->vocab[i].word, word) == 0)
            return i;
    }
    return -1;
}
int add_word(NaiveBayes* nb, const char* word) {
    if (nb->vocab_size >= MAX_VOCAB) return -1;
    strcpy(nb->vocab[nb->vocab_size].word, word);
    for (int i = 0; i < MAX_CLASSES; i++)
        nb->vocab[nb->vocab_size].count[i] = 0;
    return nb->vocab_size++;
}
void train(NaiveBayes* nb, Dataset* data) {
    nb->total_docs = data->size;
    for (int i = 0; i < data->size; i++) {
        int c = data->docs[i].label;
        if (c >= nb->num_classes) continue;
        nb->class_doc_count[c]++;
        for (int j = 0; j < data->docs[i].word_count; j++) {
            int idx = find_word(nb, data->docs[i].words[j]);
            if (idx == -1)
                idx = add_word(nb, data->docs[i].words[j]);
            if (idx != -1) {
                nb->vocab[idx].count[c]++;
                nb->total_words_in_class[c]++;
            }
        }
    }
}
double log_class_prob(NaiveBayes* nb, int c) {
    if (nb->class_doc_count[c] == 0) return -INFINITY;
    return log((double)nb->class_doc_count[c] / nb->total_docs);
}
double log_word_prob(NaiveBayes* nb, int word_idx, int c) {
    double numerator = nb->vocab[word_idx].count[c] + nb->alpha;
    double denominator = nb->total_words_in_class[c] + nb->alpha * nb->vocab_size;
    if (denominator == 0) return -INFINITY;
    return log(numerator / denominator);
}
int predict(NaiveBayes* nb, Document* doc) {
    double best_score = -INFINITY;
    int best_class = -1;
    for (int c = 0; c < nb->num_classes; c++) {
        double score = log_class_prob(nb, c);
        if (score == -INFINITY) continue;
        for (int i = 0; i < doc->word_count; i++) {
            int idx = find_word(nb, doc->words[i]);
            if (idx != -1)
                score += log_word_prob(nb, idx, c);
            else {
                double prob = nb->alpha / (nb->total_words_in_class[c] + nb->alpha * nb->vocab_size);
                if (prob > 0)
                    score += log(prob);
            }
        }
        if (score > best_score) {
            best_score = score;
            best_class = c;
        }
    }
    return best_class;
}
void update_metrics(Metrics* m, int true_label, int pred_label, int positive_class) {
    if (true_label == positive_class && pred_label == positive_class) m->TP++;
    else if (true_label != positive_class && pred_label == positive_class) m->FP++;
    else if (true_label != positive_class && pred_label != positive_class) m->TN++;
    else if (true_label == positive_class && pred_label != positive_class) m->FN++;
}
void print_metrics(Metrics* m) {
    int total = m->TP + m->TN + m->FP + m->FN;
    if (total == 0) {
        printf("No data for metrics\n");
        return;
    }
    double accuracy = (double)(m->TP + m->TN) / total;
    double precision = (m->TP + m->FP) ? (double)m->TP / (m->TP + m->FP) : 0;
    double recall = (m->TP + m->FN) ? (double)m->TP / (m->TP + m->FN) : 0;
    double f1 = (precision + recall) ? 2 * precision * recall / (precision + recall) : 0;
    printf("\nMetrics\n");
    printf("Accuracy: %.4f\n", accuracy);
    printf("Precision: %.4f\n", precision);
    printf("Recall: %.4f\n", recall);
    printf("F1-score: %.4f\n", f1);
}
int load_data(const char* filename, Dataset* data) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: cannot open file %s\n", filename);
        return 0;
    }
    char line[10000];
    data->size = 0;
    while (fgets(line, sizeof(line), file)) {
        if (data->size >= MAX_DOCS) break;
        Document* doc = &data->docs[data->size];
        doc->word_count = 0;
        char* token = strtok(line, " \n");
        if (!token) continue;
        doc->label = atoi(token);
        while ((token = strtok(NULL, " \n")) != NULL) {
            if (doc->word_count < MAX_WORDS)
                strcpy(doc->words[doc->word_count++], token);
        }
        data->size++;
    }
    fclose(file);
    return 1;
}
int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s data.txt\n", argv[0]);
        return 1;
    }
    Dataset data = { 0 };
    if (!load_data(argv[1], &data)) {
        return 1;
    }
    if (data.size < 2) {
        printf("Not enough data\n");
        return 1;
    }
    int split = (int)(data.size * 0.8);
    Dataset train_data = { 0 }, test_data = { 0 };
    for (int i = 0; i < data.size; i++) {
        if (i < split)
            train_data.docs[train_data.size++] = data.docs[i];
        else
            test_data.docs[test_data.size++] = data.docs[i];
    }
    NaiveBayes nb = { 0 };
    nb.alpha = 1.0;
    nb.num_classes = MAX_CLASSES;
    train(&nb, &train_data);
    Metrics m = { 0 };
    for (int i = 0; i < test_data.size; i++) {
        int pred = predict(&nb, &test_data.docs[i]);
        int true_label = test_data.docs[i].label;
        update_metrics(&m, true_label, pred, 1); 
    }
    print_metrics(&m);
    return 0;
}

