import json

ori_txt_path = "/Users/yxhu/Desktop/orb/new_orb/result.txt"
angle_bias_total = {"1a":0,"2a":0,"2anew":0,"3a":0,"4a":0,"5a":0}
angle_bias_cnt = {"1a":0,"2a":0,"2anew":0,"3a":0,"4a":0,"5a":0}
angle_group_bias_max = {"1a":0,"2a":0,"2anew":0,"3a":0,"4a":0,"5a":0}

x_bias_total = {"1x":0,"2x":0,"2xnew":0,"3x":0,"4x":0,"5x":0}
x_bias_cnt = {"1x":0,"2x":0,"2xnew":0,"3x":0,"4x":0,"5x":0}
x_group_bias_max = {"1x":0,"2x":0,"2xnew":0,"3x":0,"4x":0,"5x":0}
# 读取文件，每一行的数据为，文件路径，angle，xbias
# 读取groundtruth的json文件，获取对应的angle和xbias
with open('/Users/yxhu/Desktop/orb/test_orb/new_clean.json', 'r') as f:
    groundtruth = json.load(f)
with open(ori_txt_path, 'r', encoding='utf-8') as f:
    lines = f.readlines()

for line in lines:
    line = line.strip()
    line = line.split(' ')
    linelis = [i for i in line]
    pic_ori_name = linelis[0].split('/')[-1]
    pic_name = pic_ori_name.split('.jpg')[0]
    name_xbias = pic_name.split('_')[1]
    name_angle = pic_name.split('_')[-1]
    layer = pic_name.split('_')[0][0]
    if name_angle != '0':
        orb_val = float(linelis[1])
        case = layer + "a"
        gt_val = groundtruth[case][pic_ori_name]['new_standardize_angle']
        if case == '2a':
            if pic_name.split('_')[0] == '2new':
                case = '2anew'
        angle_bias_total[case] += abs(orb_val - gt_val)
        if abs(orb_val - gt_val) > angle_group_bias_max[case]:
            angle_group_bias_max[case] = round(abs(orb_val - gt_val),3)
        angle_bias_cnt[case] += 1
        if abs(orb_val - gt_val) >= 0.3:
            print(pic_name,orb_val,round(abs(orb_val-gt_val),3),"degree")
    if name_angle == '0' and name_xbias != '0':
        orb_val = float(linelis[2])
        case = layer + "x"
        gt_val = groundtruth[case][pic_ori_name]['new_standardize_xbias']
        if case == '2x':
            if pic_name.split('_')[0] == '2new':
                case = '2xnew'
        x_bias_total[case] += abs(orb_val - gt_val)
        if abs(orb_val - gt_val) > x_group_bias_max[case]:
            x_group_bias_max[case] = round(abs(orb_val - gt_val),3)
            # print("max",pic_name,orb_val,round(abs(orb_val-gt_val),3) )
        x_bias_cnt[case] += 1
        if abs(orb_val - gt_val) >= 0.3:
            print(pic_name,orb_val,round(abs(orb_val-gt_val),3) ,"mm")
    else:
        continue
for key in angle_bias_total.keys():

    print(key, round(angle_bias_total[key]/angle_bias_cnt[key],3),"degree")
for key in x_bias_total.keys():
    print(key, round(x_bias_total[key]/x_bias_cnt[key],3),"mm")
anglec=0
xc=0
print(angle_bias_cnt)
for val in angle_bias_cnt.values():
    anglec += val
print(x_bias_cnt)
for val in x_bias_cnt.values():
    xc += val
print(anglec,xc)

print(angle_group_bias_max)
print(x_group_bias_max)