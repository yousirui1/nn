#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "base.h"
#include "matrix.h"
#include "nn.h"
#include "input_layer.h"
#include "fc_layer.h"
#include "bn_layer.h"
#include "conv_layer.h"
#include "conv_transpose_layer.h"
#include "lstm_layer.h"
#include "activation_layer.h"

typedef struct {
    char* name;
    char* type;
    uint32_t num_weights;
    uint32_t* shape_lengths;
    uint32_t** shapes;
    float** weights;
} Layer;

void free_layer(Layer* layer) {
    free(layer->name);
    free(layer->type);
    free(layer->shape_lengths);
    for (int i = 0; i < layer->num_weights; i++) {
        free(layer->shapes[i]);
        free(layer->weights[i]);
    }   
    free(layer->shapes);
    free(layer->weights);
}

void read_string(FILE* file, char** str) {
    uint32_t len;
    fread(&len, sizeof(uint32_t), 1, file);
    *str = (char*)malloc(len + 1); 
    fread(*str, sizeof(char), len, file);
    (*str)[len] = '\0';
}

void read_layer(FILE* file, Layer* layer) {
    read_string(file, &layer->name);
    read_string(file, &layer->type);
    fread(&layer->num_weights, sizeof(uint32_t), 1, file);

    layer->shape_lengths = (uint32_t*)malloc(layer->num_weights * sizeof(uint32_t));
    layer->shapes = (uint32_t**)malloc(layer->num_weights * sizeof(uint32_t*));
    layer->weights = (float**)malloc(layer->num_weights * sizeof(float*));

    for (int i = 0; i < layer->num_weights; i++) {
        fread(&layer->shape_lengths[i], sizeof(uint32_t), 1, file);
        layer->shapes[i] = (uint32_t*)malloc(layer->shape_lengths[i] * sizeof(uint32_t));
        for (int j = 0; j < layer->shape_lengths[i]; j++) {
            fread(&layer->shapes[i][j], sizeof(uint32_t), 1, file);
        }
    }   

    for (int i = 0; i < layer->num_weights; i++) {
        uint32_t num_elements = 1;
        for (int j = 0; j < layer->shape_lengths[i]; j++) {
            num_elements *= layer->shapes[i][j];
        }

        layer->weights[i] = (float*)malloc(num_elements * sizeof(float));
        fread(layer->weights[i], sizeof(float), num_elements, file);
    }
}

void nn_load_weight(struct nn_t *nn)
{
    FILE* file = fopen("weights.bin", "rb");
    if (file == NULL) {
        printf("Failed to open file.\n");
        return 1;
    }

    uint32_t num_layers;
    fread(&num_layers, sizeof(uint32_t), 1, file);

    Layer* layers = (Layer*)malloc(num_layers * sizeof(Layer));
	shape_t weight_shape[10];

    struct layer_t *layer = list_first_entry(&nn->layer_root, struct layer_t, link);
    layer = list_next_entry(layer, link);
    for (int i = 0; i < num_layers; i++) {
        read_layer(file, &layers[i]);
#if 0
        printf("Layer %d:\n", i + 1);
        printf("  Name: %s\n", layers[i].name);
        printf("  Type: %s\n", layers[i].type);
        printf("  Num Weights: %d\n", layers[i].num_weights);
#endif

        for (int j = 0; j < layers[i].num_weights; j++) {
            //printf("  Weight %d:\n", j + 1);
            //printf("    Shape: ");
			weight_shape[j].size = 1;
			weight_shape[j].num_dims = layers[i].shape_lengths[j];
            for (int k = 0; k < layers[i].shape_lengths[j]; k++) {
                //printf("%d ", layers[i].shapes[j][k]);
				weight_shape[j].dims[k] = layers[i].shapes[j][k];	
			    weight_shape[j].size *= layers[i].shapes[j][k];
            }
            //printf("\n");
        }

        if(layers[i].num_weights > 0)
        {
            struct conv_weight_t conv_weight;
            struct bn_weight_t bn_weight;
            struct actiation_weight_t p_relu_weight;
            
            if(STRPREFIX(layers[i].type, "Conv2D"))
            {
                conv_weight.weights = matrix_alloc_empty(weight_shape[0]);
                conv_weight.bias = matrix_alloc_empty(weight_shape[1]); 
                conv_weight.weights->data = layers[i].weights[0];
                conv_weight.bias->data = layers[i].weights[1];
                if(STRPREFIX(layer->layer_name, "conv_2"))
                {
                    matrix_print(conv_weight.weights);
                    matrix_print(conv_weight.bias);
                }

                layer->load_weight(layer, &conv_weight);
            }
            else if(STRPREFIX(layers[i].type, "BatchNormalization"))
            {
               	bn_weight.weights = matrix_alloc_empty(weight_shape[0]); 
    			bn_weight.bias = matrix_alloc_empty(weight_shape[1]); 
    			bn_weight.mean = matrix_alloc_empty(weight_shape[2]); 
    			bn_weight.variance = matrix_alloc_empty(weight_shape[3]); 

    			bn_weight.weights->data = layers[i].weights[0];
    			bn_weight.bias->data = layers[i].weights[1];
    			bn_weight.mean->data = layers[i].weights[2];
    			bn_weight.variance->data = layers[i].weights[3];

				layer->load_weight(layer, &bn_weight);
            }
            else if(STRPREFIX(layers[i].type,"PReLU"))
            {
    			p_relu_weight.weights = matrix_alloc_empty(weight_shape[0]); 
    			p_relu_weight.weights->data = layers[i].weights[0];

                layer->load_weight(layer, &p_relu_weight);
            }
            else if(STRPREFIX(layers[i].type, "Conv2DTranspose"))
            {
#if 0
				conv_transpose_weight.weights = matrix_alloc_empty(last_shape[0], num_elements[0]/last_shape[0]);
    			conv_transpose_weight.weights->data = layers[i].weights[0];;
                layer->load_weight();
                layer->load_weight(layer, &conv_transpose_weight);
#endif
            }
            else
            {
                continue;
            }
            printf("layer->name %s \n", layer->layer_name);
            layer = list_next_entry(layer, link);
            if(list_is_last(&layer->link, &nn->layer_root))
            {
                free(layers);
                fclose(file);
                return 0;
            }
        }

        //free_layer(&layers[i]);
    }

    free(layers);
    fclose(file);
    return 0;
}

