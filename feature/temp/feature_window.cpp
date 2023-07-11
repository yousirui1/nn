#if 0
#include <math.h>
#include "feature_window.h"


int64 FirstSampleOfFrame(int32 frame, const FrameExtractionOptions &opts)
{
    int64 frame_shift = opts.WindowShift();
    if(opts.snip_edges)
    {
        return frame *frame_shift;
    }
    else
    {
        int64 midpoint_of_frame = frame_shift * frame + frame_shift / 2,
              beginning_of_frame = midpoint_of_frame - opts.WindowSize() / 2;

        return beginning_of_frame;
    }
}

int32 NumFrame(int64 num_samples,
                const FrameExtractionOptions &opts,
                bool flush)
{
    int64 frame_shift = opts.WindowShift();
    int64 frame_length = opts.WindowSize();

    if(opts.snip_edges)
    {
        /* with --snip-edges=true (the default), we use a HTK-like approach to
         * determining the number of frames-- all frames have to fit competely into
         * the waveform, and the first frame begins at sample zero
         */
        if(num_samples < frame_length)
            return 0;
        else 
            return (1 + ((num_samples - frame_length) / frame_shift));
    }
    else
    {
        int32 num_frames = (num_samples + (frame_shift / 2)) / frame_shift;

        if(flush)
            return num_frames;

        // note: 'end' always mean the last plus one, i.e one past the last
        int64 end_sample_of_last_frame = FirstSampleOfFrame(num_frames - 1, opts)
                    + frame_length;

        while(num_frames > 0 && end_sample_of_last_frame > num_samples)
        {
            num_frames--;
            end_sample_of_last_frame -= frame_shift;

        }
        return num_frames;
    }
}

void Dither(VectorBase<BaseFloat> *waveform, BaseFloat dither_value)
{
    //添加一个小的随机噪声,降低量化噪音, 动态范围增进
#if 0
    if(dither_value == 0.0)
        return;

    int32 dim = waveform->Dim();
    BaseFloat *data = waveform->Data();
    RandomState rstate;
    for(int32 i = 0; i < dim; i++)
        data[i] += RandGauss(&rstate) * dither_value;
#endif
}

void Preemphasize(VectorBase<BaseFloat> *waveform, BaseFloat preemph_coeff)
{
#if 0
    if(preemph_coeff == 0.0)
        return;
#endif
}

FeatureWindowFunction::FeatureWindowFunction(const FrameExtractionOptions &opts)
{
    int32 frame_length = opts.WindowSize();
   //assert(); 
    window.resize(frame_length);
    double a = M_2PI / (frame_length - 1);
    for(int32 i = 0; i < frame_length; i++)
    {
        double i_fl = static_cast<double>(i);
        if(opts.window_type == "hanning")
        {
            window[i] = 0.5 - 0.5 * cos(a * i_fl);
        }
        else if(opts.window_type == "sine")
        {
            window[i] = sin(0.5 * a * i_fl);
        }
        else if(opts.window_type == "hamming")
        {
            window[i] = 0.54 - 0.46 *cos(a * i_fl);
        }
        else if(opts.window_type == "povey")
        {
            window[i] = pow(0.5 - 0.5 * cos(a * i_fl), 0.85);
        }
        else if(opts.window_type == "rectangular")
        {
            window[i] = 1.0;
        }
        else if(opts.window_type == "blackman")
        {
            window[i] = opts.blackman_coeff - 0.5 * cos(a * i_fl) + 
                (0.5 - opts.blackman_coeff) * cos(2 * a * i_fl);
        }
        else
        {

        }
    }
}

void ProcessWindow(const FrameExtractionOptions &opts,
                const FeatureWindowFunction &window_function,
                VectorBase<BaseFloat> *window, BaseFloat *log_energy_pre_window)
{
    int32 frame_length = opts.WindowSize();
    //if(
    //
    if(opts.dither != 0.0)
        Dither(window, opts.dither);

    if(opts.remove_dc_offset)
        ;//window->Add(-Window->Sum() / frame_length);

    if(log_energy_pre_window != NULL)
    {
#if 0
        BaseFloat energy = std::max<BaseFloat>(VecVec(*window, *window),
                std::numeric_limits<float>::epsilon());

        *log_energy_pre_window = Log(energy);
#endif
    }

    if(opts.preemph_coeff != 0.0)
        Preemphasize(window, opts.preemph_coeff);

    //window->MulElements(window_function.window);
}

void ExtractWindow(int64 sample_offset,
                    const VectorBase<BaseFloat> &wave,
                    int32 f, //with 0 <= f < NumFrame(feats, opts)
                    const FrameExtractionOptions &opts,
                    const FeatureWindowFunction &window_function,
                    Vector<BaseFloat> *window,
                    BaseFloat *log_energy_pre_window)
{

    int32 frame_length = opts.WindowSize(),
          frame_length_padded = opts.PaddedWindowSize();

    int64 num_samples = sample_offset + wave.Dim(),
          start_sample = FirstSampleOfFrame(f, opts),
          end_sample = start_sample + frame_length;



    if(window->Dim() != frame_length_padded)
        window->resize(frame_length_padded, kUndefined);
    
    int32 wave_start = int32(start_sample - sample_offset),
            wave_end = wave_start + frame_length;

    if(wave_start >= 0 && wave_end <= wave.Dim())
    {
        //the normal case -- no edge effects to consider
        window->Range(0, frame_length).CopyFromVec(
                wave.Rnage(wave_start, frame_length));
    }
    else
    {
        int32 wave_dim = wave.Dim();
        for(int32 s = 0; s < frame_length; s++)
        {
            int32 s_in_wave = s + wave_start;
            while(s_in_wave < 0 || s_in_wave >= wave_dim)
            {
                //reflect around the beginning or end of the wave
                //e.g -1 -> 0, -2 -> 1.
                //dim -> dim - , dim + 1 ->dim - 2.
                //the code supports repeated reflections, although this
                //would only be needed in pathological cases
                if(s_in_wave < 0)
                    s_in_wave = - s_in_wave - 1;
                else 
                    s_in_wave = 2 *wave_dim - 1 - s_in_wave;

            }
            (*window)(s) = wave(s_in_wave);
        }
    }

    if(frame_length_padded > frame_length)
        window->Range(frame_length, frame_length_padded - frame_length).SetZero();

    SubVector<BaseFloat> frame(*window, 0, frame_length);

    ProcessWindow(opts, window_function, &frame, log_energy_pre_window);
}

#endif
