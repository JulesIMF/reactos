lines = []
with open('widgets.txt', 'r') as txt:
    lines = [line.strip() for line in txt]
    lines.append('')

# print(lines)
# print(len(lines))

with open('widgets.dot', 'w') as dot:
    i = 0
    print("digraph\n{\ndpi = 300;\nrankdir=\"LR\";", file=dot)
    while True:
        if i == len(lines):
            break

        idx = int(lines[i], base=16)
        i += 1
        my_type = int(lines[i], base=16)
        i += 1
        # print(i, 'inc', lines[i])
        
        my_type_str = ""
        if my_type == 0:
            my_type_str = "OUT"
        elif my_type == 1:
            my_type_str = "IN"
        elif my_type == 2:
            my_type_str = "MIXER"
        elif my_type == 3:
            my_type_str = "SELECT"
        elif my_type == 4:
            my_type_str = "PIN COMPLEX"
        elif my_type == 5:
            my_type_str = "POWER"
        elif my_type == 6:
            my_type_str = "VOLUME"
        elif my_type == 7:
            my_type_str = "BEEP"
        elif my_type == 8:
            my_type_str = "PIN?"
        else:
            my_type_str = "?"
        
        print(f'nid{idx} [label = "{hex(idx)}\\n{my_type_str}", shape=\"rectangle\"];', file=dot)
        while i < len(lines) and lines[i] != "":
            print(f"nid{int(lines[i], base=16)}->nid{idx};", file=dot)
            i += 1
            # print(i, "in while", lines[i])
        
        i += 1
        # print(i, 'while exit', lines[i])
    
    print('}', file=dot)
    
              
# print(int('e', base=16))
