#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <conio.h>
#include <unistd.h>

#define hashSize 2000

typedef struct node{
    char name[25];
    char genre[12];
    int hash_key;
    unsigned int stock;
    struct node *left;
    struct node *right;
    int height;
}game;

unsigned int hash(char *str) {
  unsigned int hash_val = 0;
  for (int i = 0; str[i] != '\0'; i++) {
    hash_val = hash_val + (unsigned int) str[i];
  }
  return hash_val % hashSize;
}

int max(int a, int b){
    if(a>b) return a;
    return b;
}

int height(game *head){
    if (head == NULL) return 0;
    return head->height;
}

int getBalance(game *head){
    if (head == NULL) return 0;
    return height(head->left) - height(head->right);
}

game* name_search(game *root,char *search_name){
    int search_key = hash(search_name);
    game *curr = root;
    while(curr!=NULL){
        if(curr->hash_key == search_key && !strcmp(search_name, curr->name)){
            return curr;
        }else if(curr->hash_key<search_key){
            curr = curr->right;
        }else{
            curr = curr->left;
        }
    }
    return NULL;
}

game *rightRotate(game *error_node){
    game *left = error_node->left;
    game *left_right = left->right;

    left->right = error_node;
    error_node->left = left_right;
 
    error_node->height = max(height(error_node->left),height(error_node->right)) + 1;
    left->height = max(height(left->left), height(left->right)) + 1;
 
    return left;
}

game *leftRotate(game *error_node){
    game *right = error_node->right;
    game *right_left = right->left;
 
    right->left = error_node;
    error_node->right = right_left;
 
    error_node->height = max(height(error_node->left),height(error_node->right)) + 1;
    right->height = max(height(right->left),height(right->right)) + 1;

    return right;
}

game *insert(game *head, char *game_title_in, char *game_genre_in, int stock_in, int hash_key_in)
{
    if(head==NULL){
        game *newnode = (game*) malloc(sizeof(game));
        strcpy(newnode->name, game_title_in); strcpy(newnode->genre, game_genre_in);
        newnode->stock = stock_in; newnode->hash_key = hash_key_in;
        newnode->left = NULL;
        newnode->right = NULL;
        newnode->height = 1;
        return newnode;
    }

    if(hash_key_in < head->hash_key) head->left = insert(head->left, game_title_in, game_genre_in, stock_in, hash_key_in);
    else if(hash_key_in > head->hash_key) head->right = insert(head->right, game_title_in, game_genre_in, stock_in, hash_key_in);

    head->height = max(height(head->left), height(head->right)) + 1;
    int b_factor = getBalance(head);

    if(b_factor>1 && hash_key_in < head->left->hash_key) return rightRotate(head);
    else if(b_factor<-1 && hash_key_in > head->right->hash_key) return leftRotate(head);
    else if(b_factor > 1 && hash_key_in > head->left->hash_key){
        head->left = leftRotate(head->left);
        return rightRotate(head);
    }else if(b_factor < -1 && hash_key_in < head->right->hash_key){
        head->right = rightRotate(head->right);
        return leftRotate(head);
    }

    return head;

}

void print_header(){
    printf("=========================================================\n");
    printf("| Game Title                | Game Genre   | Game Stock |\n");
    printf("=========================================================\n");
    return;
}

void print(game *head){
    if(head==NULL) return;
    print(head->right);
    printf("| %-25s | %-12s | %-10d |\n", head->name, head->genre, head->stock);
    printf("---------------------------------------------------------\n");
    print(head->left);
    return;
}

game * minValueNode(game* node){
    game* current = node;
    while (current->left != NULL) current = current->left;
    return current;
}
 
game* deleteGame(game* head, char* name) {
    if (head == NULL) return head;

    if (strcmp(name, head->name) < 0) head->left = deleteGame(head->left, name);
    else if (strcmp(name, head->name) > 0) head->right = deleteGame(head->right, name);

    else {
        if (head->left == NULL || head->right == NULL) { //Node has no child or only one child
            game* temp = head->left ? head->left : head->right;
            
            if (temp == NULL) { // No child case
                free(head);
                head = NULL;
            } else // One child case
                *head = *temp;

            free(temp);
        } 
        else {

            game* temp = minValueNode(head->right); //Node has two children
            strcpy(head->name, temp->name);
            strcpy(head->genre, temp->genre);
            head->stock = temp->stock;
            head->right = deleteGame(head->right, temp->name);
        }
    }
    if (head == NULL)
        return head;

    head->height = 1 + max(height(head->left), height(head->right));
    int balance = getBalance(head);

    if (balance > 1 && getBalance(head->left) >= 0)return rightRotate(head);
    
    else if (balance > 1 && getBalance(head->left) < 0) {
        head->left = leftRotate(head->left);
        return rightRotate(head);
    }
    else if (balance < -1 && getBalance(head->right) <= 0) return leftRotate(head);
    
    else if (balance < -1 && getBalance(head->right) > 0) {
        head->right = rightRotate(head->right);
        return leftRotate(head);
    }

    return head;
}

void modify_stock(game *root){
        printf("Input game title: ");
        char mod[30]; scanf("%[^\n]", mod); getchar();
        game *temp = name_search(root, mod);
        if(temp==NULL){
            printf("Data not found !\n");
            return;
        }
        printf("Current stock: %d\n\n", temp->stock);
        char ch[12];
        while(true){
            printf("Input update type[add|remove][case sensitive]: ");
            scanf("%[^\n]", ch); getchar();
            if(!strcmp(ch, "add") || !strcmp(ch, "remove")) break;
        }
        if(!strcmp(ch, "add")){
            printf("Input stock to add: ");
            int add; scanf("%d", &add); getchar();
            temp->stock+=add;
            printf("Data updated successfully\n");
        }else{
            int remove;
            while(remove<=0 || remove>temp->stock){
                printf("Input stock to remove[1-%d]: ", temp->stock);
                scanf("%d", &remove); getchar();    
            }
            if(remove==temp->stock){
                root = deleteGame(root, mod);
                printf("Data updated successfully !\n%s is removed from the warehouse !\n", mod);
                return;
            }else{
                temp->stock -= remove;
                printf("Data updated successfully !\n");
                return;
            }
        }
}

void print_menu(){
    printf("Bluejack GShop\n");
    printf("==============\n");
    printf("1. Insert Game\n");
    printf("2. View Game\n");
    printf("3. Update Stock\n");
    printf("4. Exit\n");
    printf(">> ");
    return;
}

int main(){
    game temp = {.name = "World of Warcraft", .genre = "RPG", //preset data
                .stock = 10, .hash_key = hash(temp.name), 
                .height = 1, .left = NULL, .right = NULL};
    game *root = malloc(sizeof(game));
    memcpy(root, &temp, sizeof(game));
    root = insert(root, "Valorant", "Action", 20, hash("Valorant"));
    root = insert(root, "Poker", "Card Game", 30, hash("Poker"));
    root = insert(root, "Breath of The Wild", "Adventure", 40, hash("Zelda: Breath of The Wild"));
    
    while(true){ //main loop
        int ch;
        system("cls");
        print_menu();
        scanf("%d", &ch); getchar();
        if(ch==1){
            system("cls");
            char name_inp[30], genre_inp[12];
            int stock_inp, key_inp;
            while(true){ //name input
                printf("Input game title[5-25][unique]: ");
                scanf("%[^\n]", name_inp); getchar();
                if(strlen(name_inp)>=5 && strlen(name_inp)<=25){
                    break;
                }
            }
            
            if(name_search(root, name_inp)!=NULL && root!=NULL){ //validate unique name or not
                printf("Data existed!\n"); system("pause");
                continue;
            }

            while(true){ //genre input
                printf("Input game type[Action|RPG|Adventure|Card Game]: ");
                scanf("%[^\n]", genre_inp); getchar();
                if(!strcmp(genre_inp, "Action") || !strcmp(genre_inp, "Adventure") || !strcmp(genre_inp, "RPG") || !strcmp(genre_inp, "Card Game")) break;
                else continue;
            }

            while(true){ //stock input
                printf("Input game stock[>= 1]: ");
                scanf("%d", &stock_inp); getchar();
                if(stock_inp>=1) break;
                else continue;
            }
            
            key_inp = hash(name_inp); //set key
            
            root = insert(root, name_inp, genre_inp, stock_inp, key_inp);
            printf("Insert Success !\n");
        }
        else if(ch==2){
            system("cls");
            if(root==NULL || !isalpha(root->name[0])){
                printf("Warehouse is empty !\n");
            }else{
                print_header();
                print(root);
            }
        }
        else if(ch==3){
            modify_stock(root);
        }
        system("pause");
    }
    
    return 0;
}