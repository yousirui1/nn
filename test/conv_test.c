#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define OUTPUT_CHANNELS 4
#define FILTER_HEIGHT 2
#define FILTER_WIDTH 3
#define STRIDE_Y 1
#define STRIDE_X 2
#define PADDING 0

#define INPUT_HEIGHT 2
#define INPUT_WIDTH 64
#define INPUT_CHANNELS 4

float input_data[] = {
     0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 
    0., 0., 0., 0., 

   7.023503, 3.0933914, 1.3400929, 1.4856474, 
    1.560324, 2.2858543, 0.6451563, 2.2054403, 
    0.83961046, 1.1453607, 1.0228043, 1.2050468, 
    1.192066, 0.7531548, 1.591937, 0.8457037, 
    0.97502345, 0.09336884, 1.326143, 1.1196502, 
    0.96947193, 0.03085821, 1.3473451, 0.8663765, 
    1.8753693, 0.77888846, 2.7789288, 1.4295844, 
    1.1048119, 0.63998437, 1.6176417, 1.4948972, 
    1.0846529, 1.2130079, 1.6732605, 1.2574284, 
    0.6073074, 0.06800767, 0.7230154, 0.8268324, 
    0.46473572, 0.5956136, 1.7966423, 1.2050772, 
    0.35444042, 0.24984328, 1.5228839, 1.3355874, 
    0.8435052, 0.26450315, 1.4925902, 0.749575, 
    0.09241173, 0.24492785, 0.8522312, 0.9724363, 
    0.18338741, 0.34092617, 1.0534437, 1.051826, 
    0.10541648, 0.43221557, 1.2022403, 0.7219634, 
    0.61872095, 0.5819175, 1.2600641, 0.80612123, 
    0.26003778, 0.38350666, 1.4448727, 0.9330721, 
    0.0793346, 0.1188719, 1.3275707, 0.9602964, 
    0.19733925, 0.2984714, 0.7953621, 0.7168686, 
    0.19611728, 0.37832457, 1.661526, 1.0188948, 
    0.76326525, 0.3708976, 2.0098052, 1.0819473, 
    0.3577799, 0.2504817, 0.8560641, 0.73966074, 
    0.74097353, 0.05851866, 0.61502993, 1.0675745, 
    0.21282251, 0.1781075, 0.80909514, 0.37640023, 
   -0.05459203, 0.46603367, 1.0375898, 0.8612939, 
   -0.34514576, 0.1267726, 1.2977285, 0.7936969, 
    0.01761252, 0.5750674, 1.1566066, 0.99557555, 
    0.0155122, 0.4709636, 0.73090106, 0.7437799, 
   -0.23597665, 0.5363289, 0.60745025, 0.85765254, 
   -0.8283666, 0.18842192, 0.65036947, 0.86984706, 
   -0.37202424, 0.07509921, 0.47688428, 0.5057929, 
    0.37768394, 0.21551004, 0.60135627, 0.43210912, 
    0.44893664, 0.73273915, 1.1900651, 0.29805958, 
    0.40779102, 1.1326611, 1.2564366, 0.64402586, 
   -0.44304824, 0.73226804, 0.783953, 0.58868533, 
   -0.84328264, 0.35401025, 0.7107303, 0.7835655, 
   -0.82249796, 0.03010399, 0.79192656, 0.62534654, 
   -0.72958076, 0.4376769, 0.6160539, 0.63445276, 
   -0.7270704, 0.5120755, 0.44983044, 0.67432207, 
   -1.3187888, 0.36011055, 0.5312594, 0.38568422, 
   -0.28167623, 0.34258184, 0.59871596, 0.22245489, 
    0.04109803, 0.5870551, 0.8490997, 0.5779457, 
   -0.6012028, 0.5292028, 0.15303326, 0.47338045, 
   -0.59495795, 0.3477589, 0.24446595, 0.31730932, 
   -0.15287751, 0.23505484, 0.67106265, 0.56730175, 
   -0.3270548, 0.5031752, 0.1884436, 0.6401666, 
   -0.33222383, 0.45126292, 0.30481058, 0.4128245, 
   -0.72484607, 0.46075356, 0.5105268, 0.2938517, 
   -0.42694768, 0.37706345, 0.38849714, 0.256855, 
   -0.13948633, 0.89731663, 0.01195782, 0.34311014, 
   -0.6888064, 0.12515333, -0.19313528, 0.01688471, 
   -0.7971488, 0.59772825, -0.07132458, -0.00975701, 
   -0.78635126, 0.44529933, -0.01221858, 0.08001745, 
   -0.8770671, 0.6037572, 0.24467826, 0.44060507, 
   -0.12593418, 0.46243992, 0.03866708, 0.46060893, 
    0.57152104, 0.10876781, 0.7974239, 0.04073678, 
    0.02424985, 0.73184425, 0.67220616, 0.77802706, 
   -0.8729347, 0.21379817, 0.5663631, 0.4514132, 
   -1.3010825, 0.5702492, 0.19115168, 0.6631067, 
   -1.0527538, 0.52077025, 0.26074344, 0.5305923, 
   -1.8187835, 1.1661248, -0.96681786, 0.41437584, 
    0.10917473, 0.24662128, -1.8677826, -0.12375899, 
    0.74556994, 0.06813565, 0.7829501, -0.11488677, 
    0., 0., 0., 0., 
};

float weights[] = {

-0.37037426, -0.12710443, 0.10650888, -0.16297925, 
   -0.09411036, -0.04806954, 0.19384587, 0.03358347, 
   -0.10739864, -0.05991981, 0.33357996, -0.19780788, 
   -0.08800033, 0.0100067, -0.33730364, -0.26272416, 

  -0.15644844, 0.03978436, 0.06586818, -0.86103106, 
   -0.1970513, -0.02905872, 0.70307565, -0.08334776, 
   -0.04071162, 0.03692905, 0.24753936, -0.15872677, 
    0.15743347, 0.2428962, -0.69046825, -0.7613043, 

  -0.03293051, -0.04199009, -0.07507934, -0.6249988, 
   -0.19196385, -0.1904098, 0.33746177, 0.09404358, 
    0.06538429, -0.00478652, 0.00195389, -0.12856348, 
    0.10783943, 0.10891587, -0.31877944, -0.5604718, 


 -0.26289177, 0.2118842, 0.3345644, -0.07720026, 
   -0.48303407, 0.04314772, 0.2258413, 0.0059541, 
   -0.9257015, -0.11100126, 0.5991875, 0.03159042, 
   -0.08462637, 0.04113232, -0.37555638, 0.3204538, 

  -0.37668723, -1.4885463, 0.3843776, 0.43736038, 
   -0.8109973, 0.0292057, 0.8470622, -0.12534016, 
   -1.4153349, 0.07213798, 0.32592258, 0.02623556, 
   -0.11203477, -0.34344435, -0.64458036, 0.99577683, 

   0.17698011, -1.3715473, 0.05100162, 0.38929778, 
   -0.50076824, -0.2763481, 0.3687856, -0.07700273, 
   -0.6205004, 0.0454707, -0.00640667, -0.065849, 
    0.15975152, -0.02949882, -0.1181052, 0.6944506, 
};

float bias[] = {
	-0.19020075,  0.4013968,  -0.0636001,  -0.29340032
};


void conv2D(float *input_data, float *weights, float bias[4], float *output_data) {
int out_height = (INPUT_HEIGHT - FILTER_HEIGHT + 2 * PADDING) / STRIDE_Y + 1;
int out_width = (INPUT_WIDTH - FILTER_WIDTH + 2 * PADDING) / STRIDE_X + 1;
out_width = 32;
//(64 - 3 + 2 * 0 / 2 + 1)
//61/2 30 + 1
//2, 64, 4
//1, 32, 4
printf("out_height %d out_width %d \n", out_height, out_width);
int input_data_idx, weights_idx, output_data_idx;

for (int n = 0; n < 1; n++) {
    for (int h = 0; h < out_height; h++) {
        for (int w = 0; w < out_width; w++) {
            for (int oc = 0; oc < OUTPUT_CHANNELS; oc++) {
                output_data_idx = n * (out_height * out_width * OUTPUT_CHANNELS) + h * (out_width * OUTPUT_CHANNELS) + w * OUTPUT_CHANNELS + oc;
                output_data[output_data_idx] = bias[oc];

                for (int kh = 0; kh < FILTER_HEIGHT; kh++) {
                    for (int kw = 0; kw < FILTER_WIDTH; kw++) {
                        for (int ic = 0; ic < INPUT_CHANNELS; ic++) {
                            int input_h = h * STRIDE_Y + kh - PADDING;
                            int input_w = w * STRIDE_X + kw - PADDING;

                            if (input_h >= 0 && input_h < INPUT_HEIGHT && input_w >= 0 && input_w < INPUT_WIDTH) {
                                input_data_idx = n * (INPUT_HEIGHT * INPUT_WIDTH * INPUT_CHANNELS) + input_h * (INPUT_WIDTH * INPUT_CHANNELS) + input_w * INPUT_CHANNELS + ic;
                                weights_idx = kh * (FILTER_WIDTH * INPUT_CHANNELS * OUTPUT_CHANNELS) + kw * (INPUT_CHANNELS * OUTPUT_CHANNELS) + ic * OUTPUT_CHANNELS + oc;
                                output_data[output_data_idx] += input_data[input_data_idx] * weights[weights_idx];
                            }
                        }
                    }
                }
            }
        }
    }   
}
}

float output_data[1][1][32][4];

int main() {
    // 执行Conv2D操作
    conv2D(input_data, weights, bias, output_data);

    // 打印输出结果
    for (int n = 0; n < 1; n++) {
        for (int h = 0; h < 1; h++) {
            for (int w = 0; w < 32; w++) {
                for (int oc = 0; oc < 4; oc++) {
                    printf("%f ", output_data[n][h][w][oc]);
                }
                printf("\n");
            }
            printf("\n");
        }
    }   

    return 0;
}
