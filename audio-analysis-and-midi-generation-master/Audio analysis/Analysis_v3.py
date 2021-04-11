import numpy as np
from matplotlib import pyplot as plt
import scipy.io.wavfile as wav
from numpy.lib import stride_tricks
from scipy.signal import find_peaks
from midiutil import MIDIFile


def stft(sig, frameSize, overlapFac=0.5, window=np.hanning):
    win = window(frameSize)
    hopSize = int(frameSize - np.floor(overlapFac * frameSize))

    samples = np.append(np.zeros(int(np.floor(frameSize/2.0))), sig)

    cols = np.ceil((len(samples) - frameSize) / float(hopSize)) + 1

    samples = np.append(samples, np.zeros(frameSize))

    frames = stride_tricks.as_strided(samples, shape=(int(cols), frameSize), strides=(samples.strides[0]*hopSize, samples.strides[0])).copy()
    frames *= win

    return np.fft.rfft(frames)


def logscale_spec(spec, sr=44100, factor=20.):
    timebins, freqbins = np.shape(spec)

    scale = np.linspace(0, 1, freqbins) ** factor
    scale *= (freqbins-1)/max(scale)
    scale = np.unique(np.round(scale))

    newspec = np.complex128(np.zeros([timebins, len(scale)]))
    for i in range(0, len(scale)):
        if i == len(scale)-1:
            newspec[:, i] = np.sum(spec[:, int(scale[i]):], axis=1)
        else:
            newspec[:, i] = np.sum(spec[:, int(scale[i]):int(scale[i+1])], axis=1)

    allfreqs = np.abs(np.fft.fftfreq(freqbins*2, 1./sr)[:freqbins+1])
    freqs = []
    for i in range(0, len(scale)):
        if i == len(scale)-1:
            freqs += [np.mean(allfreqs[int(scale[i]):])]
        else:
            freqs += [np.mean(allfreqs[int(scale[i]):int(scale[i+1])])]

    return newspec, freqs


def plotstft(audiopath, binsize=2**13, colormap="jet"):
    samplerate, samples = wav.read(audiopath)

    s = stft(samples, binsize)

    sshow, freq = logscale_spec(s, factor=1.0, sr=samplerate)

    ims = np.abs(sshow)
    #ims = 20.*np.log10(np.abs(sshow/10e-6)) # amplitude to decibel
    maxi = np.max(ims)
    mini = np.min(ims)
    SNR = (maxi-mini)/4                                                                #SNR
    timebins, freqbins = np.shape(ims)

    freq_res = samplerate/freqbins/2
    time_res = samples.shape[0]/samplerate/timebins

    print("timebins: ", timebins)
    print("freqbins: ", freqbins)
    print("Freq resolution: ", freq_res, "[Hz]")
    print("Time resolution: ", time_res, "[s]")

    plt.figure(figsize=(15, 7.5))
    plt.imshow(np.transpose(np.abs(sshow)), origin="lower", aspect="auto", cmap=colormap, interpolation="none")
    plt.colorbar()

    plt.xlabel("time (s)")
    plt.ylabel("frequency (hz)")
    plt.xlim([0, timebins-1])
    ylim = freqbins * 2 * 1000 / samplerate
    plt.ylim([0, ylim])
    # plt.ylim([0, freqbins])

    xlocs = np.float32(np.linspace(0, timebins-1, 5))
    plt.xticks(xlocs, ["%.02f" % l for l in ((xlocs*len(samples)/timebins)+(0.5*binsize))/samplerate])
    ylocs = np.int16(np.round(np.linspace(0, ylim-1, 10)))
    plt.yticks(ylocs, ["%.02f" % freq[i] for i in ylocs])


    return ims, binsize, freq, samplerate, samples, freq_res, SNR


def traces(stft, freq, SNR, samplerate, samples, binsize=2**13):

    timebins, freqbins = np.shape(stft)

    tab = np.full((freqbins, timebins), np.NINF)
    ims = np.transpose(stft)

    for i in range(0, len(ims[:][0])):
        peaks, _ = find_peaks(ims[:, i], height=SNR)
        for j in range(0, len(peaks)):
            tab[peaks[j], i] = ims[peaks[j], i]

    plt.figure(figsize=(15, 7.5))
    plt.imshow(tab, origin="lower", aspect="auto", cmap="jet", interpolation="none")
    plt.colorbar()

    plt.xlabel("time (s)")
    plt.ylabel("frequency (hz)")
    plt.xlim([0, timebins - 1])
    ylim = freqbins * 2 * 1000 / samplerate
    plt.ylim([0, ylim])
    # plt.ylim([0, freqbins])

    xlocs = np.float32(np.linspace(0, timebins - 1, 5))
    plt.xticks(xlocs, ["%.02f" % l for l in ((xlocs * len(samples) / timebins) + (0.5 * binsize)) / samplerate])
    ylocs = np.int16(np.round(np.linspace(0, ylim - 1, 10)))
    plt.yticks(ylocs, ["%.02f" % freq[i] for i in ylocs])
    plt.show()
    plt.clf()

    np.savetxt('freqval.txt', tab)
    #np.savetxt('ims.txt', ims)

    return tab


def filemon(tab):

    time_size, freq_size = np.shape(tab)

    midi_tab = np.full((time_size, freq_size), 0)
    freq_tab = np.full((time_size, freq_size), 0)

    new_freq_tab = []
    new_midi_tab = []

    for i in range(0, len(tab[:][0])):
        for j in range(0, len(tab)):
            if tab[j][i] != np.NINF and freq_res*j <= 1500:
                midi_tab[j][i] = tab[j][i]
                freq_tab[j][i] = freq_res*j

    for i in range(len(midi_tab)):
        if np.sum(midi_tab[:][i]) != 0:
            new_midi_tab.append(midi_tab[:][i])
            new_freq_tab.append(freq_tab[:][i])

    # np.savetxt('onlygain.txt', new_midi_tab)
    # np.savetxt('onlyfreqs.txt', new_freq_tab)

    return new_midi_tab, new_freq_tab


def freq_extrude(tab):

    if len(tab) != 0:
        time_size, freq_size = np.shape(tab)
        freq_tab_opt = np.full((time_size, freq_size), 0)

        notes = [0, 82.41, 87.31, 92.5, 98, 103.83, 110, 116.54, 123.47, 130.81, 138.59, 146.83, 155.56, 164.81, 174.61, 185,
                196, 207.65, 220, 233.08, 246.94, 261.63, 277.18, 293.66, 311.13, 329.63, 349.23, 369.99, 392, 415.3, 440,
                466.16, 493.88, 523.25, 554.37, 587.33, 622.25, 659.26, 698.46, 739.99, 783.99, 830.61, 880, 932.33, 987.77,
                1046.5, 1108.73, 1174.66, 1244.51, 1318.51, 1396.91, 1479.98, 1567.98]

        for i in range(len(tab)):
            for j in range(0, len(tab[:][0])):
                if tab[i][j] != 0:
                    absolute_difference_function = lambda list_value: abs(list_value - tab[i][j])
                    closest_value = min(notes, key=absolute_difference_function)
                    freq_tab_opt[i][j] = closest_value

        # np.savetxt('onlyfreqs_opt.txt', freq_tab_opt)
        return freq_tab_opt
    else:
        exit()


def freqtomidi(fInHz, fA4InHz=440):
    midi = np.asarray(fInHz)
    for i in range(0, len(fInHz)):
        for j in range(0, len(fInHz[:][0])):
            if fInHz[i][j] == 0:
                midi[i][j] = 0
            else:
                midi[i][j] = 70 + 12 * np.log2(fInHz[i][j] / fA4InHz)
    # np.savetxt('MIDI_tab.txt', midi)
    return midi


def one_timebin_err(gain_tab, freq_tab):

    time_size, freq_size = np.shape(freq_tab)
    gain_no_err = np.full((time_size, freq_size), 0)
    freq_no_err = np.full((time_size, freq_size), 0)

    for i in range(0, len(freq_tab[:][0])):
        for j in range(0, len(freq_tab)):
            if (j == 0 and j == len(freq_tab)) or (freq_tab[j][i-1] != 0 and freq_tab[j][i+1] != 0):
                gain_no_err[j][i] = gain_tab[j][i]
                freq_no_err[j][i] = freq_tab[j][i]

    np.savetxt('No_err_gain.txt', gain_no_err)
    np.savetxt('No_err_Midi.txt', freq_no_err)
    return gain_no_err, freq_no_err

def createMIDIfile(midi_table):
    track = 0
    channel = 0
    time = 0  # In beats
    duration = 1  # In beats
    tempo = 120  # In BPM
    volume = 100  # 0-127, as per the MIDI standard

    MyMIDI = MIDIFile(1)  # One track, defaults to format 1 (tempo track is created # automatically)
    MyMIDI.addTempo(track, time, tempo)

    for i in range(len(midi_table)):
        degrees = midi_table[i]

        for j, pitch in enumerate(degrees):
            MyMIDI.addNote(track, channel, pitch, time+i, duration, volume)

    with open("MIDI_OUT.mid", "wb") as output_file:
        MyMIDI.writeFile(output_file)


if __name__ == "__main__":

    filename = "Real_Piano_2.wav"
    if filename.endswith('.wav'):
        ims, binsize, freq, samplerate, samples, freq_res, SNR = plotstft(filename)
        freqs = traces(ims, freq, SNR, samplerate, samples)

        gain_tab, freq_tab = filemon(freqs)
        freq_tab_opt = freq_extrude(freq_tab)
        MIDI_tab = freqtomidi(freq_tab_opt)
        noerror_gain, noerror_MIDI = one_timebin_err(gain_tab, MIDI_tab)


    else:
        print("Enter valid file")

    MIDI_tab_2 = np.transpose(noerror_MIDI)
    createMIDIfile(MIDI_tab_2)
