#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ANSI COLOR CODES
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"


// STRING CLASS DEFINITION
typedef struct {
    char* value;
    int length;
    int hidden_length;
} string;

string STRING(char* value) {
    string s;
    s.length = strlen(value);
    s.hidden_length = s.length * 2;
    s.value = malloc(s.hidden_length);
    strcpy(s.value, value);
    return s;
}

char at(const string* s, int index) {
    return s->value[index];
}

string substr(const string* s, int start, int end) {
    string sub;
    sub.length = end - start;
    sub.hidden_length = sub.length * 2;
    sub.value = malloc(sub.hidden_length);
    strncpy(sub.value, s->value + start, sub.length);
    return sub;
}

void append_cstr(string* s, const char* app) {
    int app_length = strlen(app);
    int new_length = s->length + app_length;
    if (s->hidden_length < new_length) {
        s->hidden_length = new_length * 2;
        s->value = realloc(s->value, s->hidden_length);
    }
    strcat(s->value, app);
    s->length = new_length;
}

void append(string* s, const string* app) {
    append_cstr(s, app->value);
}

void delete(string* s) {
    free(s->value);
    s->value = NULL;
    s->length = 0;
    s->hidden_length = 0;
}

void print(const string* s) {
    printf("%s", s->value);
}

void printC(const string* s, const char* color) {
    printf("%s", color);
    printf("%s", s->value);
    printf("%s", RESET);
}

void replace(string* s, char old, char new) {
    for (int i = 0; i < s->length; ++i) {
        if (s->value[i] == old) {
            s->value[i] = new;
        }
    }
}

void split(const string* s, char delim, string** str_array, int* str_array_length) {
    
    // determine number of splits
    int i = 0;
    int count = 1;
    while (i < s->length) {
        if (s->value[i] == delim) {
            ++count;
        }
        ++i;
    }

    *str_array = (string*)malloc(count * sizeof(string));
    *str_array_length = count;

    // split string
    int start = 0;
    int end = -1;
    int index = 0;
    int j = 0;
    while (j < s->length) {
        if (s->value[j] == delim) {
            end = j;
            (*str_array)[index] = substr(s, start, end);
            ++index;
            start = end + 1;
        }
        ++j;
    }
    end = j;
    (*str_array)[index] = substr(s, start, end);
}
// STRING CLASS DEFINITION


void printCC(char s, const char* color) {
    printf("%s", color);
    printf("%c", s);
    printf("%s", RESET);
}

void nl() {
    printf("\n");
}

void print_indent(int indent) {
    for (int i = 0; i < indent; ++i) {
        printf("%s", "  ");
    }
}

void pretty_json(const string* json, int indent) {

    // find first { or [
    int i = 0;
    char c_char = at(json, i);
    while(c_char != '{' && c_char != '[' && i < json->length) {
        c_char = at(json, i);
        ++i;
    }

    // if no brackets are found print out the key value pairs
    if (i == json->length && c_char != '}' && c_char != ']') {

        string* lines = NULL;
        int lines_length;
        split(json, ',', &lines, &lines_length);

        for (int i = 0; i < lines_length; ++i) {
            // split line by :
            string* line = &lines[i];

            string* key_value;
            int key_value_length;
            split(line, ':', &key_value, &key_value_length);

            // print pairs
            print_indent(indent);
            printC(&key_value[0], YELLOW);
            printf("%s", ": ");
            printC(&key_value[1], CYAN);
            printf("%s", ",");
            nl();

            delete(&key_value[0]);
            delete(&key_value[1]);
            delete(line);
        }
        
    }
    // otherwise match with enclosing } or ] and recurse
    else {

        int start = i;
        char open = c_char;
        char close = c_char == '{' ? '}' : ']';
        int open_brackets = 1;
        while(i < json->length && open_brackets != 0) {
            ++i;
            c_char = at(json, i);
            if (c_char == open) {
                ++open_brackets;
            } else if (c_char == close) {
                --open_brackets;
            }
        }

        int end = i;

        // print out first bracket
        string json_inner = substr(json, start + 1, end - 1);
        print_indent(indent);
        if (open == '{') {
            printCC(open, BLUE);
            nl();
        } else {
            printCC(open, GREEN);
            nl();
        }

        // recurse enclosed data
        pretty_json(&json_inner, indent + 1);
        delete(&json_inner);


        // print out last bracket
        print_indent(indent);
        if (close == '}') {
            printCC(close, BLUE);
            // printf("%s", BLUE);
            // printf("%s", "}");
            // printf("%s", RESET);
            nl();
        } else {
            printCC(close, GREEN);
            nl();
        }
    }


    // add those with current indent

    // recurse with incremented indent for inner text

}


int main(int argc, char *argv[]) {
    
    string json = STRING(argv[1]);
    // string json = STRING("{ \"name\": \"bob\" }");
    replace(&json, '\n', ' ');

    // print(&json);

    pretty_json(&json, 0);

    return 0;
}