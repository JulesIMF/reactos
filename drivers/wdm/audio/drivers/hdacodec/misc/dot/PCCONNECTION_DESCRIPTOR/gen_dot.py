#!/usr/bin/python3

import re
from typing import List
import subprocess

pattern = re.compile("{\s*(\w*),\s*(\w*),\s*(\w*),\s*(\w*)\s*}")

with open('topo.txt', 'r') as txt:
    topo = txt.read()


class Connection:
    def __init__(self,
                 fromNode,
                 fromPin,
                 toNode,
                 toPin):
        self.fromNode = fromNode
        self.fromPin = fromPin
        self.toNode = toNode
        self.toPin = toPin


connections: List[Connection] = []
nodes = set()

print("Found connections:")
for i, (fn, fp, tn, tp) in enumerate(pattern.findall(topo)):
    print(f"{i}: {fn}[{fp}] ---> {tn}[{tp}]")
    if fn == "PCFILTER_NODE":
        fn = "PCFILTER_NODE_F"
    if tn == "PCFILTER_NODE":
        tn = "PCFILTER_NODE_T"

    connections.append(Connection(fn, fp, tn, tp))
    nodes.add(fn)
    nodes.add(tn)

print(nodes)
outPins = {x : set() for x in nodes}
inPins = {x : set() for x in nodes}

for connection in connections:
    outPins[connection.fromNode].add(connection.fromPin)
    inPins[connection.toNode].add(connection.toPin)

print(outPins)
print(inPins)

with open('topo.dot', 'w') as dot:
    print("digraph\n{\ndpi = 300;\nrankdir=\"HR\";", file=dot)

    for node in nodes:
        label = ""
        if len(inPins[node]):
            label += "{"
            for pin in inPins[node]:
                label += f" <in_{node}_{pin}> {pin} |"
            label = label[:-1] + "} | "

        label += node

        if len(outPins[node]):
            label += " | {"
            for pin in outPins[node]:
                label += f"<out_{node}_{pin}> {pin} |"
            label = label[:-1] + "}"

        print(f"    node_{node} [shape=record, label=\"{label}\"];", file=dot)
    for c in connections:
        print(
            f"    node_{c.fromNode}:<out_{c.fromNode}_{c.fromPin}> -> node_{c.toNode}:<in_{c.toNode}_{c.toPin}>;", file=dot)

    print("}", file=dot)

subprocess.run(["dot", "-Tpng", "topo.dot", "-o", "topo.png"], stdout=subprocess.DEVNULL)
