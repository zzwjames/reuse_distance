import time
start=time.time()

def count(reuse_dis):
  if(reuse_dis>10 and reuse_dis<20):
    global count_10_20
    count_10_20+=1
  else:
    global count_100
    count_100+=1


with open("/content/data.txt","r",encoding='utf-8') as file:
  indexs=file.readlines()
  length=len(indexs)
  for i in range(length):
    cur_index=indexs[i]
    cur_set={cur_index}
    j=i+1
    while j<length and indexs[j]!=cur_index:
      cur_set.add(indexs[j])
      j+=1
    cur_reuse_dis=len(cur_set)-1
    print(cur_reuse_dis)
    count(cur_reuse_dis)

end=time.time()
print(end-start)