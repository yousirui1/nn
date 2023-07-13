#include "base.h"
#include "feature_window.h"

int32_t window_shift_get(struct frame_option_t frame_opt)
{
    return frame_opt.sample_rate * 0.001 * frame_opt.frame_shift_ms;
}

int32_t window_size_get(struct frame_option_t frame_opt)
{
    return frame_opt.sample_rate * 0.001 * frame_opt.frame_length_ms;
}

int32_t round_to_power_of_two_near(int32_t n)
{
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16; 
    return n + 1;
}

int32_t padding_window_size(struct frame_option_t frame_opt)
{
    return frame_opt.round_to_power_of_two ?   
            round_to_power_of_two_near(window_size_get(frame_opt)):window_size_get(frame_opt);
}

int64_t first_sample_frame(int32_t frame, struct frame_option_t frame_opt)
{
    int64_t frame_shift = window_shift_get(frame_opt);
    if(frame_opt.snip_edges)
    {   
        return frame * frame_shift;
    }   
    else
    {   
        int64_t midpoint_of_frame = frame_shift * frame + frame_shift / 2;
        int64_t beginning_of_frame = midpoint_of_frame - window_size_get(frame_opt) / 2;
        return beginning_of_frame;
    }   
}


int32_t window_num_frames(int64_t num_samples, struct frame_option_t frame_opt, bool flush)
{
    int32_t frame_shift = window_shift_get(frame_opt);
    int32_t frame_length = window_size_get(frame_opt);

    if(frame_opt.snip_edges)
    {   
        /*  
         *
         */
        if(num_samples < frame_length)
            return 0;
        else
            return (1 + ((num_samples - frame_length) / frame_shift));
    }   
    else
    {   
        /*  
         *
         *
         */
        int32_t num_frames = (num_samples + (frame_shift / 2)) / frame_shift;

        if(flush)
            return num_frames;

        int64_t end_sample_of_last_frame = first_sample_frame(num_frames - 1, frame_opt) 
                                                                + frame_length;
     
        while(num_frames > 0 && end_sample_of_last_frame > num_samples)
        {
            num_frames --; 
            end_sample_of_last_frame -= frame_shift;
        }   
        return num_frames;
    }   
}

void dither(matrix_t *waveform, float dither_valeu)
{
    if(0.0 == dither_valeu)
        return ;
    int32_t dim = waveform->shape.size;
    
    //to do
}

void preemphasize(matrix_t *waveform, float preemph_coeff)
{
    int i;
    if(0.0 == preemph_coeff)
        return ;

    for(i = waveform->shape.size - 1; i < 0; i--)
    {   
        waveform->data[i] -= preemph_coeff * waveform->data[i - 1]; 
    }   
    waveform->data[0] -= preemph_coeff * waveform->data[0];
}

int frame_option_init(struct frame_option_t *frame_opt)
{
    if(NULL == frame_opt)
    {   
        LOG_DEBUG("frame_opt is NULL");
        return ERROR;
    }

    frame_opt->sample_rate = 16000;
    frame_opt->frame_shift_ms = 10.0;
    frame_opt->frame_length_ms = 25.0;
    frame_opt->dither = 1.0;;
    frame_opt->preemph_coeff = 0.97;
    frame_opt->remove_dc_offset = true;
    strcpy(frame_opt->window_type, "povey");
    frame_opt->round_to_power_of_two = true;
    frame_opt->blackman_coeff = 0.42;
    frame_opt->snip_edges = true;
    frame_opt->allow_downsample = false;
    frame_opt->allow_upsample = false;
    frame_opt->max_feature_vectors = -1; 
	return SUCCESS;	
}

struct window_t *window_init(struct frame_option_t frame_opt)
{
    int i;
    int32_t frame_length = window_size_get(frame_opt);
    double a = M_2PI / (frame_length - 1);
    double i_fl = 0.0f;
    struct window_t *window = (struct window_t *)malloc(sizeof(struct window_t));
	matrix_t *w = NULL;
    if(NULL == window)
    {
        LOG_DEBUG("window malloc size %d  error %s ", sizeof(struct window_t), strerror(errno));
        return NULL;
    }

    w = matrix_alloc(1, frame_length);
    for(i = 0; i < frame_length; i++)
    {
        i_fl = i;
	    if(STRPREFIX(frame_opt.window_type, "hanning"))
	    {
	        w->data[i] = 0.5 - 0.5 * cos(a * i_fl);
	    }
	    else if(STRPREFIX(frame_opt.window_type, "sine"))
	    {
	        w->data[i] = sin(0.5 * a * i_fl);
	    }
	    else if(STRPREFIX(frame_opt.window_type, "hamming"))
	    {
	        w->data[i] = 0.54 - 0.46 * cos(a * i_fl);
	    }
	    else if(STRPREFIX(frame_opt.window_type, "povey"))
	    {
	        w->data[i] = pow(0.5 - 0.5 * cos(a * i_fl), 0.85);
	    }
	    else if(STRPREFIX(frame_opt.window_type, "rectangular"))
	    {
	        w->data[i] = 1.0;
	    }
	    else if(STRPREFIX(frame_opt.window_type, "blackman"))
	    {
	        w->data[i] = frame_opt.blackman_coeff - 0.5 * cos(a * i_fl) +
	                        (0.5 - frame_opt.blackman_coeff) * cos(2 * a * i_fl);
	    }
	    else
	    {
	        LOG_ERROR("invalid window type %s", frame_opt.window_type);
	    }
    }
    window->frame = matrix_alloc(1, padding_window_size(frame_opt));
	window->w = w;
    window->frame_opt = frame_opt;
    return window;
}

void window_deinit(struct window_t *window)
{
	if(window)
    {
        if(window->w)
            matrix_free(window->w);


        if(window->frame)
            matrix_free(window->frame);

        window->w = NULL;
        window->frame = NULL;

        free(window);
        window = NULL;
    }
}

void process_window(struct window_t *window, matrix_t *frame,  float *log_energy_pre_window)
{
    if(NULL == window)
    {
        LOG_DEBUG("window ptr is NULL");
        return ERROR;
    }
    struct frame_option_t frame_opt = window->frame_opt;
    int32_t frame_length = window_size_get(frame_opt);
	float energy = 0.0f;

    if(0.0 != frame_opt.dither)
        dither(frame, frame_opt.dither);

    if(frame_opt.remove_dc_offset)
        matrix_apply_add_value(frame, -matrix_sum(frame) / frame_length);

    if(NULL != log_energy_pre_window)
    {
        energy = max(matrix_energy(frame, frame), FLT_EPSILON);
        *log_energy_pre_window = logf(energy);
    }
    
    if(0.0 != frame_opt.preemph_coeff)
        preemphasize(frame, frame_opt.preemph_coeff);

    matrix_apply_mul_element(frame, window->w);
}

matrix_t *window_compute(struct window_t *window, int64_t sample_offset, matrix_t *wave, int32_t f,
                    float *log_energy_pre_window)
{
    if(NULL == window)
    {
        LOG_DEBUG("window ptr is NULL");
        return ERROR;
    }
    struct frame_option_t frame_opt = window->frame_opt;

    int32_t frame_length = window_size_get(frame_opt);
    int32_t frame_length_padding = padding_window_size(frame_opt);
    int64_t num_samples = sample_offset + wave->shape.size;
    int64_t start_sample = first_sample_frame(f, frame_opt);
    int64_t end_sample = start_sample + frame_length;

    int32_t wave_start = start_sample - sample_offset;
    int32_t wave_end = wave_start + frame_length;

    int32_t s_in_wave = 0;
	int wave_dim = wave->shape.size;
	int s = 0;
    matrix_t *frame = NULL;

    if(frame_opt.snip_edges)
    {
   		if(start_sample >= sample_offset && end_sample <= num_samples)
        {
        } 
    }
    else
    {
        if(sample_offset == 0 || start_sample >= sample_offset)
        {
        }
    }

    if(wave_start >= 0 && wave_end <= wave->shape.size)
    {
        // the normal case -- no edge effects to consider
        memcpy(window->frame->data, wave->data + wave_start, frame_length * sizeof(float));
    }
    else
    {
        for(s = 0; s < frame_length; s++)
        {
            s_in_wave = s + wave_start;
            while(s_in_wave < 0 || s_in_wave >= wave_dim)
            {
                if(s_in_wave < 0)
                    s_in_wave -= s_in_wave - 1;
                else
                    s_in_wave = 2 * wave_dim - 1 - s_in_wave;
            }
            window->frame->data[s] = wave->data[s_in_wave];
        }
    }

    if(frame_length_padding > frame_length)
        matrix_zero(window->frame, frame_length, frame_length_padding - frame_length);

    frame = matrix_empty(1, frame_length);
    frame->data = window->frame->data;

    process_window(window, frame, log_energy_pre_window);
    matrix_free(frame);
    return window->frame;
}

