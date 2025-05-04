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

# Practice prints
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

msigA = move_average(sA,20)

basic_fft(rate_A,tA,[sA,msigA],'Signal A vs Time')

