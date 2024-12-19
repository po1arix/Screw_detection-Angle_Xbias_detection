txt_path = './r1.txt'

cnt = {# T,F
    "0":[0,0],
    "1":[0,0],
    "2":[0,0],
    "3":[0,0],
    "4":[0,0],
    "5":[0,0]
}
def gt(screwt, caset):
    if screwt == "z":
        if caset == "1":
            return "1"
        if caset == "2":
            return "5"
        if caset == "3":
            return "3"
    else:
        if caset == "1":
            return "0"
        if caset == "2":
            return "2"
        if caset == "3":
            return "4"

with open(txt_path, "r") as f:
    d = f.readlines()
for line in d:
    line = line.strip()
    linel = line.split(" ")
    filename = linel[0].split("/")[-1]
    pred = linel[1]
    sp_name = filename.split("_")
    screwt = filename[0]
    caset = sp_name[1].split("-")[-1][0]

    ground_truth = gt(screwt,caset)
    if ground_truth == pred:
        cnt[ground_truth][0]+=1
    else:
        cnt[ground_truth][1]+=1
print(cnt)
total_t = 0
total_f = 0
total = 0
for val in cnt.values():
    total_t += val[0]
    total_f += val[1]
total = total_t + total_f
acc = total_t / total
print(f"total:{total} , acc = {round(acc,4)}")