#ifndef __FEATURE-WINDOW_H__
#define __FEATURE-WINDOW_H__

#include <map>
#include <string>


struct FrameExtractionOptions {

    BaseFloat samp_freq;
    BaseFloat frame_shift_ms;
    BaseFloat frame_length_ms;
    BaseFloat dither;
    BaseFloat preemph_coeff;
    bool remove_dc_offset;
    std::string window_type;    
    bool round_to_power_of_two;
    BaseFloat blackman_coeff;
    bool snip_edges;
    bool allow_downsample;
    bool allow_upsample;
    FrameExtractionOptions():
        samp_freq(16000),
        frame_shift_ms(10.0),
        frame_length_ms(25.0),
        dither(1.0),
        preemph_coeff(0.97),
        remove_dc_offset(true),
        window_type("povey"),
        round_to_power_of_two(true),
        blackman_coeff(0.42),
        snip_edges(true),
        allow_downsample(false),
        allow_upsample(false),
        max_feature_vectors(-1)
        { }

    void Register(OptionsItf *opts)
    {



    }
    int32 WindowShift() const {
        return static_cast<int32>(samp_freq * 0.001 * frame_shift_ms);
    }

    int32 WindowSize() const {
        return static_cast<int32>(samp_freq * 0.001 * frame_length_ms);
    }

    int32 PaddedWindowSize() const {
        return (round_to_power_of_two ? RoundUpToNearestPowerOfTwo(WindowSize()) :
                            WindowSize());
    }
};

struct FeatureWindowFunction {
    FeatureWindowFunction() {}

    explicit FeatureWindowFunction(const FrameExtractionOptions &opts);
    FeatureWindowFunction(const FeatureWindowFunction &other):
        window(other.window) { }
    Vecotr<BaseFloat> window;
};

int64 FirstSampleOfFrame(int32 frame, const FrameExtractionOptions &opts);

void Dither(VectorBase<BaseFloat> *waveform, BaseFloat dither_value);
void Preemphasize(VectorBase<BaseFloat> *waveform, BaseFloat preemph_coeff);

void ProcessWindow(const FrameExtractionOptions &opts,
                    const FeatureWindowFunction &window_function,
                    VectorBase<BaseFloat> *window,
                    BaseFloat *log_energy_pre_window = NULL);

void ExtractWindow(int64 sample_offset,
                    const VectorBase<BaseFloat> &wave,
                    int32 f,
                    const FrameExtractionOptions &opts,
                    const FeatureWindowFunction &window_function,
                    Vector<BaseFloat> *window,
                    BaseFloat *log_energy_pre_window = NULL);



#endif //  __FEATURE-WINDOW_H__
