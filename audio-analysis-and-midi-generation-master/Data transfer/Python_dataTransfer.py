def sendToArduino(sendStr):
    ser.write(sendStr.encode('utf-8'))

def recvFromArduino():
    global startMarker, endMarker

    ck = ""
    x = "z"
    byteCount = -1

    while ord(x) != startMarker:
        x = ser.read()

    while ord(x) != endMarker:
        if ord(x) != startMarker:
            ck = ck + x.decode("utf-8")
            byteCount += 1
        x = ser.read()

    return (ck)


# ============================

def waitForArduino():
    global startMarker, endMarker

    msg = ""
    while msg.find("Arduino is ready") == -1:

        while ser.inWaiting() == 0:
            pass

        msg = recvFromArduino()

        print(msg)
        print()


# ======================================================================

def runTest(td):
    numLoops = len(td)
    waitingForReply = False

    n = 0
    while n < numLoops:
        teststr = td[n]

        if waitingForReply == False:
            sendToArduino(teststr)
            print("Sent from PC -- LOOP NUM " + str(n) + " TEST STR " + teststr)
            waitingForReply = True

        if waitingForReply == True:

            while ser.inWaiting() == 0:
                pass

            dataRecvd = recvFromArduino()
            print("Reply Received  " + dataRecvd)
            n += 1
            waitingForReply = False

            print("===========")

        time.sleep(1)

def sendMidiTab(tab):
    testData = []
    rows2, columns2 = np.shape(tab)
    testData.append("<" + str(rows2) + " " + str(columns2) + ">")
    midi_str2 = ""
    for i in range(rows2):
        midi_str2 = ""
        for j in range(columns2):
            if j == 0:
                midi_str2 = "<" + str(tab[i][j])
            if j > 0:
                midi_str2 = midi_str2 + " " + str(tab[i][j])
            if j == columns2 - 1:
                midi_str2 = midi_str2 + ">"
        testData.append(midi_str2)
    runTest(testData)



import serial
import time
import numpy as np

print()
print()

serPort = "COM3"
baudRate = 115200
ser = serial.Serial(serPort, baudRate)
print("Serial port " + serPort + " opened  Baudrate " + str(baudRate))

startMarker = 60
endMarker = 62

waitForArduino()

midi_tab = [62, 0, 66, 0, 61]
rows = len(midi_tab)        #1D
columns = len(midi_tab)

midi_str = ""
for i in range(len(midi_tab)):
    if i == 0:
        midi_str = "<" + str(midi_tab[i])
    if i > 0:
        midi_str = midi_str + " " + str(midi_tab[i])
    if i == len(midi_tab) - 1:
        midi_str = midi_str + ">"


midi_tab2 = [[0, 0, 62, 0, 0, 0], [0, 0, 62, 0, 64, 0], [0, 64, 66, 0, 0, 68]]
sendMidiTab(midi_tab2)
ser.close