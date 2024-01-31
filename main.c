#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

/*EZGI AKKKAYA*/


//TASK 1


// Struct to store word occurrences
struct WordInfo {
    char *word;
    int count;
};

char **hamWords = NULL;  //Array to store ham words
char **spamWords = NULL; // Array to store spam words
int hamCount = 0; //counter for the number of ham words
int spamCount = 0;  // counter for the number of spsm words
int i;
char *topSpamWords[10];

// In the function below, file is opened and readen
void loadData() {
    FILE *fp;
    char line[256];
    fp = fopen("preprocessed_dataset.txt", "r");

    if(fp == NULL) {
        perror("Error opening file");
        return;
    }
// Read the file line-by-line until the end
    while(fgets(line, 256, fp)) {
        // Split the line into label (ham/spam) and message using tab as the delimiter
        char *label = strtok(line, "\t"); // tokenize the line to extract the label  using '\t' as a delimiter.
        char *message = strtok(NULL, "\n"); // tokenize again to extract the actual message using '\n' as a delimiter.

        // Tokenize the message by spaces to extract individual words
        if (message != NULL) {
            char *token = strtok(message, " ");
            while (token != NULL) {
                char lowerToken[200];
                for(i = 0; token[i]; i++) {
                    lowerToken[i] = tolower(token[i]);
                }
		            lowerToken[i]='\0'; // in order to ensure that the string is null terminated

// If the label is "ham",  the word  is add to the hamWords array
                if(strcmp(label, "ham") == 0) {
                    hamWords = realloc(hamWords, (hamCount + 1) * sizeof(char *));
                    hamWords[hamCount] = strdup(lowerToken);
                    //strdup function is used in order to  duplicate the word and store it in the array
                    //this way, the original word doesn't change and a copy of it is located into the array
                    hamCount++;
                } else if(strcmp(label, "spam") == 0) {
                    spamWords = realloc(spamWords, (spamCount + 1) * sizeof(char *));
                    spamWords[spamCount] = strdup(lowerToken);
                    spamCount++;
                }
//using null here helps to move on to the next word
                token = strtok(NULL, " ");
            }
        }
    }
    fclose(fp);
}

/**
 * This function searches for the number of occurrences of a given word in both 'ham' and 'spam' categories.
 */
void countOccurrence() {
    char word[100];      // Array to store the given word, a word is assumed to be not passing 100 characters
    int spamOccurrence = 0; //Counter for occurrences in 'spam' category
    int hamOccurrence = 0;
   // int i;
    printf("Please enter a word to search for: ");
    scanf("%s", word);

    for ( i = 0; word[i]; i++) {
        word[i] = tolower(word[i]);
    }
// for loop through the 'ham' words and count the occurrences of the word input
    for( i = 0; i < hamCount; i++) {
        if (strcmp(hamWords[i], word) == 0) {
            hamOccurrence++;
        }
    }

    for( i = 0; i < spamCount; i++) {
        if (strcmp(spamWords[i], word) == 0) {
            spamOccurrence++;
        }
    }

//print the results of task1
      printf("Results of task 1: \n");
    if(hamOccurrence == 0 && spamOccurrence == 0) {
        printf("This word doesn't occur in the text!\n");
    } else {
        printf("The word '%s' appears %d times in ham messages and %d times in spam messages.\n", word, hamOccurrence, spamOccurrence);
    }
}



//TASK 2

 // Compare function implemented for qsort
int compareWordInfo(const void *a, const void *b) {
    //the line below is a shorthand of 
    return ((struct WordInfo *)b)->count - ((struct WordInfo *)a)->count;

    // this comparison function returns:
// - positive value if the count of 'b' > count of 'a' ('b' should come before 'a' in the sorted order)
// - negativre value if the count of 'a' > of 'b' ('a' should come before 'b' in the sorted order)
// - 0  if the counts are equal 
}


/**
This function finds top 10 spam words*/

void findTop10SpamWords() {
    struct WordInfo *spamArray = malloc(spamCount * sizeof(struct WordInfo)); //Dynamically allocated array to hold spam word information (word and its count).
    int numWords = 0;  // keeps track of unique words added to spamArray


    // Count word occurrences in spam messages by traverse through all the spam words 
    for ( i = 0; i < spamCount; i++) {
        char *word = spamWords[i];
        int found = 0; //flag which turns 1 when it is found

        int j;
        // Check if the word is already in spamArray
        for (j = 0; j < numWords; j++) {
            if (strcmp(spamArray[j].word, word) == 0) {
                 // if the word is found, increment its count and set the found flag
                spamArray[j].count++;
                found = 1;
                break;
            }
        }

        // if the word is not found, add it to spamArray
        if (!found) {
            spamArray[numWords].word = strdup(word); //numWords's slot of the spamArray is populated with using strdup.
            //strdup function takes in a string (word) and creates a new duplicate of that string in a different memory location. 
            //It then returns the address (or pointer) to this new string.This returned pointer (address of the new duplicated string) 
            //is then stored in our array's word field, essentially pointing our structure's word field to the new string.This is particularly useful when you want to keep a copy of a string without affecting the original one. [reference: tutorialspoint.com]

            spamArray[numWords].count = 1;
            numWords++;
        }
    }

    // Sort spamArray in descending order of counts
    qsort(spamArray, numWords, sizeof(struct WordInfo), compareWordInfo);

    // Print tthe result of task2, he top 10 occurring spam words
    printf("Results of task 2: \n");
    printf("Top 10 occurring words in spam messages:\n");
    for ( i = 0; i < numWords && i < 10; i++) {
        printf("%s: %d times\n", spamArray[i].word, spamArray[i].count);
        topSpamWords[i] = strdup(spamArray[i].word);
    }



    // Free memory
    for ( i = 0; i < numWords; i++) {
        free(spamArray[i].word);
    }
    free(spamArray);
}

//TASK 3

/**
 * Check if a given message contains top spam words.
 * @param message The message string to check.
 * @return Returns 1 if the message contains at least 3 top spam words, otherwise 0.
 */

 //use the most common 10 words that are identified in Task 2 to label a message as spam or ham

 //the function below is a helper function for  spamDetector function

char* containsSpamWords(char *message) {
    int spamWordCount = 0;  //counter to keep track of how many top spam words are found in the message
    //the message is tokenized by spaces to extract individual words
    char *token = strtok(message, " ");
   while (token) {
   char lowerToken[100];
   int j;
   for(j = 0; token[j]; j++) {
      lowerToken[j] = tolower(token[j]);
   }
   lowerToken[j]='\0';
   for (i = 0; i < 10; i++) {
      if (strcmp(lowerToken, topSpamWords[i]) == 0) {
                spamWordCount++;
                
                break;  // move to next token
            }
        }
        // If we've found at least 3 top spam words in the message, return spam 
  
        token = strtok(NULL, " ");
    }
          if (spamWordCount >= 3) {
            
            return "spam";
        } else {
                return "ham";

        }
    // If we've gone through the entire message and found less than 3 top spam words, return ham, --> message is not spam
}



/**
 * Detects if messages from the testData.txt file are spam or ham using top spam words.
 * It then calculates and prints the accuracy of predictions for both spam and ham messages.
 */
void spamDetector() {
    FILE *fp = fopen("testData.txt", "r");
    char line[1024];
    int correctSpam = 0, correctHam = 0, totalSpam = 0, totalHam = 0;

    if (!fp) {
        perror("Error opening testData.txt");
        return;
    }
 // Read lines from the testData.txt file one by one
    while (fgets(line, 1024, fp)) {
        char *actualLabel = strtok(line, "\t");
        char *message = strtok(NULL, "\n");

        if (!message){
                printf("no message");
         continue;}
        // Check if the label indicates the message is spam
         char *predictedLabel = containsSpamWords(message);
        if (strcmp(actualLabel, "spam") == 0) {
            totalSpam++;
             //If the predicted label is same as the actual label,  increment the count of correctly identified spam messages
            if (strcmp(predictedLabel, "spam") == 0) {
                correctSpam++;
            }

             // If the label indicates the message is ham and,
             // If the predicted label and actual label are both "ham"  increment the count of correctly identified ham messages
        } else if(strcmp(actualLabel, "ham") == 0)  {
            totalHam++;
            if (strcmp(predictedLabel, "ham") == 0) {
                correctHam++;
            }
        }
    }

    fclose(fp);

// Calculate the accuracy for identifying spam and ham messages (#correct/#total)
    double spamAccuracy = (double)correctSpam / totalSpam * 100;
    double hamAccuracy = (double)correctHam / totalHam * 100;


//Prints the results of task 3
    printf("Results of task 3: \n");
   
    printf("Accuracy of spam predictions: %.2lf%%\n", spamAccuracy);
    printf("Accuracy of ham predictions: %.2lf%%\n", hamAccuracy);
    printf("Total spam messages: %d\n", totalSpam);
    printf("Total ham messages: %d\n", totalHam);
}

int main() {

    loadData();
    countOccurrence();
    findTop10SpamWords();
    spamDetector();


    for ( i = 0; i < hamCount; i++) {
        free(hamWords[i]);
    }
    free(hamWords);

    for (i = 0; i < spamCount; i++) {
        free(spamWords[i]);
    }
    free(spamWords);

    
     return 0;
}


//TASK 4 
/*
Observations

1. The spam detector's performance was below expectation,with an accuracy of 42% , especially for spam messages. But when the straightforward logic behind this implementation
is considered, using common words is a simple way to start building a spam checker and it didn't worked that bad.
2. One primary reason for the low accuracy in detecting spam might be the approach of using just 3 of the top 10 spam words. This method may not be comprehensive enough to accurately determine if a message is spam.
3. Another observation is the insufficiency of the sample size. The larger the sample, the more accurate our predictions could potentially be.
4. Some of the words in the top spam list might also be commonly used in regular (ham) messages. This overlap can result in false positives where ham messages are incorrectly labeled as spam.
    Some of the words that are not  in the top spam list might also belong to a spam message, and not taking them into consideration causes less spam messages than expected.
5. To improve the detector, we could:
   - Increase the sample size for better training data.
   - Adjust the threshold from 3 words, maybe using a dynamic threshold based on the overall word count of a message.
   - Consider other features such as the frequency of the spam words, the presence of certain keys, or certain patterns commonly seen in spam messages.
   - Considerin the meanings of the words, or define a set of words with potentially spammy meanings. 
   - Use a machine learning approach where the model learns from a larger dataset and makes predictions based on multiple features rather than just the presence of certain words.
   
*/

