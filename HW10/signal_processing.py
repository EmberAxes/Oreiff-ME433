# Library imports
import matplotlib.pyplot as plt # for plotting
import csv

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
    print(len(t), ", ",len(data1))
    
    return (t, data1)

tA,sA = make_csv("sigA.csv")
tB,sB = make_csv("sigB.csv")
tC,sC = make_csv("sigC.csv")
tD,sD = make_csv("sigD.csv")

# Question 2: Practice plotting -----------------------------------------------

plt.plot(tA,sA,'b',)
plt.xlabel('Time [s]')
plt.ylabel('Signal')
plt.title('Signal vs Time')
plt.show()