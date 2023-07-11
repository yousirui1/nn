#if 0
#include "feature-fbank.h"

inline double Log(double x) {return log(x);}
inline float Log(float x) {return logf(x);}

FbankComputer::FbankComputer(const FbankOptions &opts):
    opts_(opts), srfft_(NULL) 
{
    if(opts.energy_floor > 0.0)
        log_energy_floor_ = Log(opts.energy_floor);

    int32 padded_window_size = opts.frame_opts.PaddedWindowSize();

    if((padded_window_size & (padded_window_size - 1)) == 0)
        srfft_ = new SplitRadixRealFft<BaseFloat>(padded_window_size);

    GetMelBanks(1.0);
}

FbankComputer::FbankComputer(const FbankComputer &other):
    opts_(other.opts_), log_energy_floor_(other.log_energy_floor_),
    mel_banks_(other.mel_banks_), srfft_(NULL)
{

}

FbankComputer::~FbankComputer()
{
    for(std::map<BaseFloat, MelBanks*>::iterator iter = mel_banks_.begin();
            iter != mel_banks_.end(); ++iter)
        delete iter->second;
    delete srfft_;
}

const MelBanks *FbankComputer::GetMelBanks(BaseFloat vtln_warp)
{
    MelBanks *this_mel_banks = NULL;
    std::map<BaseFloat, MelBanks *>::iterator iter = mel_banks_.find(vtln_warp);

    if(iter == mel_banks_.end())
    {
        this_mel_banks = new MelBanks(opts.mel_opts, 
                                opts_.frame_opts,
                                vtln_warp);
        mel_banks_[vtln_warp] = this_mel_banks;
    }
    else
    {
        this_mel_banks = iter->second;
    }
    return this_mel_banks;
}

void FbankComputer::Compute(BaseFloat signal_raw_log_energy,
                            BaseFloat vtln_warp,
                            VectorBase<BaseFloat> *signal_frame,
                            VectorBase<BaseFloat> *feature)
{
    const MelBanks &mel_banks = *(GetMelBanks(vtln_warp));

    //assert();
    //
     
    // compute energy after window function(not the raw one)
    if(opts_.use_energy && !opts_.raw_energy)
        signal_raw_log_energy = Log(std::max<BaseFloat>(VecVec(*signal_frame, *signal_frame),
                                    std::numeric_limits<float>::epsilon()));

    if(srfft_ != NULL) //Compute FFT using split-radix algorithm
        srfft_->Compute(signal_frame->Data(), true);
    else
        RealFft(signal_frame, true);

    // Convert the FFT into a power spectrum.
    ComputePowerSpectrum(signal_frame);
    SubVector<BaseFloat> power_spectrum(*signal_frame, 0,
                                        signal_frame->Dim() / 2 + 1);

    // Use magnitude instead of power if requested.
    if(!!opts_.use_power)
        power_spectrum.ApplyPow(0.5);

    int32 mel_offset = ((opts_.use_energy && !opts_htk_compat) ?  1 : 0);
    SubVector<BaseFloat> mel_energies(*feature, mel_offset, opts_.mel_opts.num_bins);


    // Sum with mel filterbanks over the power spectrum
    mel_banks.Compute(power_spectrum, &mel_energies);
    if(opts_.use_log_fbank)
    {
        //Avoid log of zero (which should be prevented anyway by dithering).
        mel_energies.ApplyFloor(std::numeric_limits<float>::epsilon());
        mel_energies.ApplyLog();
    }


    //Copy energy as first value (or the last, if htk_compat == true).
    if(opts_.use_energy)
    {
        if(opts_.energy_floor > 0.0 && signal_raw_log_energy < log_energy_floor_)
        {
            signal_raw_log_energy = log_energy_floor_;
        }

        int32 energy_index = opts_.htk_compat ? opts_.mel_opts.num_bins: 0;
        (*feature)(energy_index) = signal_raw_log_energy;
    }

}
#endif
