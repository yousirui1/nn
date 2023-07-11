#ifndef __FEATURE-FBANK_H__
#define __FEATURE-FBANK_H__

#include "feature-window.h"

struct FbankOptions {
    FrameExtractionOptions frame_opts;
    MelBanksOptions mel_opts;
    bool use_energy;
    BaseFloat energy_floor;
    bool raw_energy;
    bool htk_compat;
    bool use_log_fbank;
    bool use_power;

    FbankOptions(): mel_opts(23),
                /* defaults 
                * this seems to be common for 16khz-sampled data,
                * but for 8khz-sampled data, 15 may be better
                * */
                use_energy(false),
                energy_floor(0.0),
                raw_energy(true),
                htk_compat(false),
                use_log_fbank(true),
                use_power(true) {}

    void Register(OptionsItf *opts)
    {
        frame_opts.Register(opts);
        mel_opts.Register(opts);
        opts->Register("use-energy", &use_energy,
                        "Add an extra dimension with energy to the FBANK output");
        opts->Register("energy-floor", &energy_floor,
                        "");
        opts->Register("raw-energy", &raw_energy,
                        "If true, compute energy before preemphasis and windowing");
        opts->Register("htk-compat", &htk_compat, "If true, put energy last.  "
                        "Warning: not sufficient to get HTK compatible features (need "
                        "to change other parameters).");
        opts->Register("use-log-fbank", &use_log_fbank,
                        "If true, produce log-filterbank, else produce linear.");
        opts->Register("use-power", &use_power,
                        "If true, use power, else use magnitude .");
    }
};

class FbankComputer {
 public:
    typedef FbankOptions Options;

    explicit FbankComputer(const FbankOptions &opts);
    FbankComputer(const FbankComputer &other);

    int32 Dim() const {
        return opts_.mel_opts.num_bins + (opts_.use_energy ? 1 : 0);
    }

    bool NeedRawLogEnergy() const { return opts_.use_energy && opts_.raw_energy;}
    
    const FrameExtractionOptions &GetFrameOptions() const {
        return opts_.frame_opts;
    }

    void Compute(BaseFloat signal_raw_log_energy,
                BaseFloat vtln_warp,
                VectorBase<BaseFloat> *signal_frame,
                VectorBase<BaseFloat> *feature);

    ~FbankComputer();

 private:
    const MelBanks *GetMelBanks(BaseFloat vtln_warp);

    FbankOptions opts_;
    BaseFloat log_energy_floor_;
    std::map<BaseFloat, MelBanks*> mel_banks_;
    SplitRadixRealFft<BaseFloat> *srfft_;

    FbankComputer &operator = (const FbankComputer &other);
};

typedef OfflineFeatureTpl<FbankComputer> Fbank;

#endif //  __FEATURE-FBANK_H__
