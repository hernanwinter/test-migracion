/*
    This example shows how to use the MITIE C API to perform named entity
    recognition.
*/
#include <stdio.h>
#include <stdlib.h>
#include <mitie.h>
#include <string.h>

// ----------------------------------------------------------------------------------------
struct TagStadistics{
	char* tag_name;
	double best_score;
	char* tag_text;
	unsigned long cant;
};

struct TagStadistics* tag_occurrences;
struct TagStadistics print_entity (char** tokens, const mitie_named_entity_detections* dets, unsigned long i, int* cant_tags);

// ----------------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    mitie_named_entity_extractor* ner = 0;
    mitie_named_entity_detections* dets = 0;
    unsigned long num_tags = 0;
    unsigned long num_dets = 0;
    unsigned long i = 0;
    char** tokens = 0;
    int return_code = EXIT_FAILURE;
    int* cant_tags;

    struct TagStadistics tags_info[4];
    struct TagStadistics current_tag;

    if (argc != 3)
    {
        printf("You must give a MITIE ner model file as the first command line argument\n");
        printf("followed by a text file to process. For example:\n");
        printf("./ner_example MITIE-models/english/ner_model.dat sample_text.txt\n");
        return EXIT_FAILURE;
    }

    ner = mitie_load_named_entity_extractor(argv[1]);
    if (!ner)
    {
        printf("Unable to load model file\n");
        goto cleanup;
    }

    // Print out what kind of tags this tagger can predict.
    num_tags = mitie_get_num_possible_ner_tags(ner);
    printf("The tagger supports %lu tags:\n", num_tags);
    for (i = 0; i < num_tags; ++i) {
        printf("   %s\n", mitie_get_named_entity_tagstr(ner, i));
        tags_info[i].tag_name = mitie_get_named_entity_tagstr(ner, i);
        tags_info[i].best_score = 0;
    }

    // Now get some text and turn it into an array of tokens.
    tokens = mitie_tokenize_file(argv[2]);
    if (!tokens)
    {
        printf("Unable to tokenize file.\n");
        goto cleanup;
    }

    // Now detect all the entities in the text file we loaded and print them to the screen.
    dets = mitie_extract_entities(ner, tokens);
    if (!dets)
    {
        printf("Unable to allocate list of MITIE entities.");
        goto cleanup;
    }

    num_dets = mitie_ner_get_num_detections(dets);
    tag_occurrences = malloc(1 * sizeof(struct TagStadistics));
    tag_occurrences[0].cant = 0;
    tag_occurrences[0].tag_text = malloc( 8 * sizeof(char *) );
    strcpy(tag_occurrences[0].tag_text, " ");

    cant_tags = malloc(1 * sizeof(int));
    *cant_tags = 1;
    printf("\nNumber of named entities detected: %lu\n", num_dets);
    for (i = 0; i < num_dets; ++i)
    {
        current_tag = print_entity(tokens, dets, i, cant_tags);
        printf("%lu\n", tag_occurrences[0].cant);

        if (strcmp(current_tag.tag_name, tags_info[0].tag_name) == 0) {
        	if (tags_info[0].best_score < current_tag.best_score) {
        		tags_info[0].best_score = current_tag.best_score;
        		tags_info[0].tag_text = current_tag.tag_text;
        	}
        } else {
        	if (strcmp(current_tag.tag_name, tags_info[1].tag_name) == 0) {
	        	if (tags_info[1].best_score < current_tag.best_score) {
	        		tags_info[1].best_score = current_tag.best_score;
	        		tags_info[1].tag_text = current_tag.tag_text;
	        	}
        	} else {
        		if (strcmp(current_tag.tag_name, tags_info[2].tag_name) == 0) {
		        	if (tags_info[2].best_score < current_tag.best_score) {
		        		tags_info[2].best_score = current_tag.best_score;
		        		tags_info[2].tag_text = current_tag.tag_text;
		        	}
        		} else {
        			if (strcmp(current_tag.tag_name, tags_info[3].tag_name) == 0) {
			        	if (tags_info[3].best_score < current_tag.best_score) {
			        		tags_info[3].best_score = current_tag.best_score;
			        		tags_info[3].tag_text = current_tag.tag_text;
			        	}
        			}
        		}
        	}
        }
    }

    printf("Tag %s: Score: %0.3f: %s\n", tags_info[0].tag_text, tags_info[0].best_score, tags_info[0].tag_name);
    printf("Tag %s: Score: %0.3f: %s\n", tags_info[1].tag_text, tags_info[1].best_score, tags_info[1].tag_name);
    printf("Tag %s: Score: %0.3f: %s\n", tags_info[2].tag_text, tags_info[2].best_score, tags_info[2].tag_name);
    printf("Tag %s: Score: %0.3f: %s\n", tags_info[3].tag_text, tags_info[3].best_score, tags_info[3].tag_name);


    return_code = EXIT_SUCCESS;
cleanup:
    mitie_free(tokens);
    mitie_free(dets);
    mitie_free(ner);

    return return_code;
}

// ----------------------------------------------------------------------------------------

struct TagStadistics print_entity (
    char** tokens,
    const mitie_named_entity_detections* dets,
    unsigned long i,
    int* cant_tags
)
{
    unsigned long pos, len;
    float score;
    char* tag_type;
    struct TagStadistics tag;
    char* dummy_text;
    unsigned long j = 0;
    int found = 0;
    struct TagStadistics* tmp;

    pos = mitie_ner_get_detection_position(dets, i);
    len = mitie_ner_get_detection_length(dets, i);
    score = mitie_ner_get_detection_score(dets,i);
    tag_type = mitie_ner_get_detection_tagstr(dets,i);
    // Print the label and score for each named entity and also the text of the named entity
    // itself.  The larger the score the more confident MITIE is in the tag.
    //printf("   Tag %lu: Score: %0.3f: %s: ", mitie_ner_get_detection_tag(dets,i), score, tag_type);
    tag.tag_text = malloc( len * 8 * sizeof(char *) );
    strcpy(tag.tag_text, " ");
    while(len > 0)
    {
    	strcat(tag.tag_text, tokens[pos]);
        pos++;
        --len;
    }

    while ((j < *cant_tags) && (found == 0)) {
    	if (strcmp(tag_occurrences[j].tag_text, tag.tag_text) == 0) {
    		tag_occurrences[j].cant++;
    		found = 1;
    	}
    	j++;
    }

    if(found == 0) {
    	if ((*cant_tags == 1) && (tag_occurrences[0].cant == 0)) {
    		tag_occurrences[0].tag_name = tag_type;
    		tag_occurrences[0].tag_text = malloc( len * 8 * sizeof(char *) );
    		strcpy(tag_occurrences[0].tag_text, " ");
    		strcat(tag_occurrences[0].tag_text, tag.tag_text);
    		tag_occurrences[0].cant++;
    	} else {
	    	tmp = realloc(tag_occurrences, ((*cant_tags) + 1) * sizeof(struct TagStadistics));
            if (tmp != NULL) {
                tag_occurrences = tmp;
                tmp = NULL;
    	    	(*cant_tags)++;
    	    	tag_occurrences[(*cant_tags) - 1].cant = 1;
    	    	tag_occurrences[(*cant_tags) - 1].tag_name = tag_type;
                tag_occurrences[(*cant_tags) - 1].tag_text = tag.tag_text;
            } else {
                printf("Error realloc");
            }
	    	//tag_occurrences[(*cant_tags) - 1].tag_text = malloc( len * 8 * sizeof(char *) );
			//strcpy(tag_occurrences[(*cant_tags) - 1].tag_text, " ");
	     	//strcat(tag_occurrences[(*cant_tags) - 1].tag_text, tag.tag_text);
	    }
    }

    tag.tag_name = tag_type;
    tag.best_score = score;
    printf("%s\n", tag.tag_text);
    tag_occurrences[0].cant ++;

    return tag;
}

// ----------------------------------------------------------------------------------------

