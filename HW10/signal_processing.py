# Library imports
import matplotlib.pyplot as plt # for plotting
import csv
import numpy as np

# Question 1: Get data into lists ---------------------------------------------

def make_csv(file):

    t = [] # column 0
    data1 = [] # column 1

    with open(file) as f:
        # open the csv file
        reader = csv.reader(f)
        for row in reader:
            # read the rows 1 one by one
            t.append(float(row[0])) # leftmost column
            data1.append(float(row[1])) # second column

    # print the data to verify it was read
    # print(len(t), ", ",len(data1))
    
    return (t, data1)

tA,sA = make_csv("sigA.csv")
tB,sB = make_csv("sigB.csv")
tC,sC = make_csv("sigC.csv")
tD,sD = make_csv("sigD.csv")

# Question 2: Practice plotting -----------------------------------------------

# plt.plot(tA,sA,'b',)
# plt.xlabel('Time [s]')
# plt.ylabel('Signal')
# plt.title('Signal vs Time')
# plt.show()

# Question 3: Calculating sample rate -----------------------------------------
rate_A = len(tA)/tA[-1]
rate_B = len(tB)/tB[-1]
rate_C = len(tC)/tC[-1]
rate_D = len(tD)/tD[-1]

# # Practice prints
# print("Sample rate A: ",rate_A)
# print("Sample rate B: ",rate_B)
# print("Sample rate C: ",rate_C)
# print("Sample rate D: ",rate_D)
# input("wait")

# Question 4: Signal vs time and FFT ------------------------------------------
def basic_fft(sample_rate,time_vector,signal_vectors,title):
    colors = ['k','r']
    fig, (ax1, ax2) = plt.subplots(2, 1)
    
    for i, signal in enumerate(signal_vectors):
        Fs = sample_rate # sample rate
        Ts = 1.0/Fs; # sampling interval
        y = signal # the data to make the fft from
        n = len(y) # length of the signal
        k = np.arange(n)
        T = n/Fs
        frq = k/T # two sides frequency range
        frq = frq[range(int(n/2))] # one side frequency range
        Y = np.fft.fft(y)/n # fft computing and normalization
        Y = Y[range(int(n/2))]

        ax1.plot(time_vector,y,colors[i],linewidth = 1)
        ax2.loglog(frq,abs(Y),colors[i],linewidth = 1) # plotting the fft

    ax1.set_xlabel('Time (sec)')
    ax1.set_ylabel('Amplitude')
    ax1.title.set_text(title)
    ax2.set_xlabel('Freq (Hz)')
    ax2.set_ylabel('|Y(freq)|')
    ax2.title.set_text('Fourier Transform')
    fig.tight_layout()
    plt.show()

# basic_fft(rate_A,tA,sA,'Signal A vs Time')
# basic_fft(rate_B,tB,sB,'Signal B vs Time')
# basic_fft(rate_C,tC,sC,'Signal C vs Time')
# basic_fft(rate_D,tD,sD,'Signal D vs Time')

# Question 5: Moving average filter -------------------------------------------

def move_average(data,avg_pts):
    avgd_data = []
    long_data = [0]*(avg_pts-1) + data

    for i in range(avg_pts-1,len(long_data)):
        avg_chunk = np.mean(long_data[i-(avg_pts-1):i])
        avgd_data.append(avg_chunk)
    
    return (avgd_data)

# msigA = move_average(sA,75)
# msigB = move_average(sB,75)
# msigC = move_average(sC,75)
# msigD = move_average(sD,75)

# basic_fft(rate_A,tA,[sA,msigA],'Signal A vs Time, 75 Averaged')
# basic_fft(rate_B,tB,[sB,msigB],'Signal B vs Time, 75 Averaged')
# basic_fft(rate_C,tC,[sC,msigC],'Signal C vs Time, 75 Averaged')
# basic_fft(rate_D,tD,[sD,msigD],'Signal D vs Time, 75 Averaged')

# Question 6: IIR filter -------------------------------------------

def iir(data,A):
    '''
    If data is noisy, make A large (0-1).
    '''
    B = 1-A
    iir_average = [0]*len(data)

    for i in range(len(data)):
        if i == 0:
            iir_average[i] = data[i]
        else:
            prev_avg = iir_average[i-1]
            new_avg = A*prev_avg + B*data[i]
            iir_average[i] = new_avg

    return (iir_average)

A = 0.98
iirA = iir(sA,A)
iirB = iir(sB,A)
iirC = iir(sC,A)
iirD = iir(sD,A)

# basic_fft(rate_A,tA,[sA,iirA],'Signal A vs Time, A = 0.98, B = 0.02')
# basic_fft(rate_B,tB,[sB,iirB],'Signal B vs Time, A = 0.98, B = 0.02')
# basic_fft(rate_C,tC,[sC,iirC],'Signal C vs Time, A = 0.98, B = 0.02')
# basic_fft(rate_D,tD,[sD,iirD],'Signal D vs Time, A = 0.98, B = 0.02')

# Question 7: Online FIIIR weights -------------------------------------------

def onlineFIIIR(data,rate,cutoff,length,mode):
    '''
    mode = 1: rectangular
    mode = 2: hamming
    mode = 3: blackman
    mode = 4: kaiser
    '''
    # Configuration.
    fS = rate  # Sampling rate. fS
    fL = cutoff  # Cutoff frequency. fL
    N = length # Filter length, must be odd. N

    # Compute sinc filter.
    h = np.sinc(2 * fL / fS * (np.arange(N) - (N - 1) / 2))

    # Apply window.
    if mode == 1:
        mode = 1
    if mode == 2:
        h *= np.hamming(N)
    if mode == 3:
        h *= np.blackman(N)
    if mode == 4:
        beta = input('Beta please:')
        h *= np.kaiser(N,beta)

    # Normalize to get unity gain.
    h /= np.sum(h)

    # Applying the filter to a signal s can be as simple as writing
    s_new = np.convolve(data, h, mode='same')

    return (s_new)

new_A = onlineFIIIR(sA,rate_A,100,91,1)
basic_fft(rate_A,tA,[sA,new_A],'Signal A vs Time, Rectangular, fL = 100, bL = 100')