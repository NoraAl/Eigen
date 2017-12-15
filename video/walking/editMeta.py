import re

file = open("meta.csv")

i = 1
l = ""

for line in file:
    if re.match("(.*)-(.*)-(.*)-(.*)-(.*)", line):
        string = line
        l = l + string
    else:
        string = "------"+line
        l = l + string
        
    if (i%25)==0:
        l = l + '\n'
    i = i + 1
    
print l

# for line in file:
#     if re.match("(.*)-(.*)-(.*)-(.*)-(.*)", line):
#         string = line
#         l = l + string
#     else:
#         string = "------"+line
#         l = l + string
        
#     if (i%25)==0:
#         l = l + '\n'
#     i = i + 1
    
# print l


# i = 1
# l = ""
# for line in file:
#     if re.match("(.*)boxing(.*)", line):
#         string = "0,"+line
#         l = l + string
#         # print string
#     elif re.match("(.*)handclapping(.*)", line):
#         string = "1,"+line
#         l = l + string
#         # print string
#     elif re.match("(.*)handwaving(.*)", line):
#         string = "2,"+line
#         l = l + string
#         # print string
#     elif re.match("(.*)jogging(.*)", line):
#         string = "3,"+line
#         l = l + string
#         # print string
#     elif re.match("(.*)running(.*)", line):
#         string = "4,"+line
#         l = l + string
#         # print string
#     elif re.match("(.*)walking(.*)", line):
#         string = "5,"+line
#         l = l + string
#         # print string
#     # elif re.match("(.*)walking(.*)", line):
#     #     # print "walking" +" ",i
    
#     if (i%25)==0:
#         l = l + '\n'
#     i = i + 1
    
# print l
