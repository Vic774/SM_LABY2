import matplotlib.pyplot as plt

f = open("signal.csv", "w")

segment_time = 200
T = 7 * segment_time
ts = 1
size = T/ts
Dc_comp = 0
Amp = 800
time = 0
total_time = 0

wektor = []

for i in range(0, int(size)):
	if((total_time % segment_time) == 0) and (total_time != 0):
		Dc_comp += 250
	if total_time == segment_time * 5:
		Dc_comp = 150
		Amp = 0
	value = (time / segment_time * Amp) + Dc_comp

	if total_time >= segment_time * 6:
		value = 0

	time += ts
	if time >= segment_time:
		time = 0
	total_time += ts

	# wektor.append(value)
	value = int(value)
	value = (value / 3300) * 4095
	f.write(hex(int(value)) + ",\n")
	wektor.append(value)
f.close()
plt.plot(wektor)
plt.show()

# f = open("signal.h", "w")
#
# f.write("#ifndef SIGNAL_H_\n")
# f.write("#define SIGNAL_H_\n\n")
# f.write("#include <stdint.h>\n\n")
# f.write("#define SIGNAL_SIZE " + str(int(size)) + "\n\n")
# f.write("extern uint16_t sygnal[SIGNAL_SIZE];\n\n")
#
# f.write("#endif /* SIGNAL_H_ */\n")
#
# f.close()
#
# f = open("signal.c", "w")
#
# f.write("#include \"signal.h\"\n\n")
# f.write("extern uint16_t sygnal[SIGNAL_SIZE] = {\n#include \"signal.csv\"\n};\n")
#
# f.close()