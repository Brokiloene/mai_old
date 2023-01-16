humanInfo = {}
childInfo = {}

with open("romanovs.ged", "r", encoding="UTF-8") as f:
    prevId, prevHusbId, prevWifeId = -1, -1, -1
    for line in f:
        line = line.strip()
        if line.split()[-1] == "INDI":
            prevId = line.split()[1]
            humanInfo[prevId] = {"name": "", "sex": ""}
            childInfo[prevId] = []
        elif line.split()[1] == "GIVN":
            humanInfo[prevId]["name"] = line[7:]
        elif line.split()[1] == "SEX":
            humanInfo[prevId]["sex"] = line.split()[-1]
        elif line.split()[1] == "HUSB":
            prevHusbId = line.split()[-1]
        elif line.split()[1] == "WIFE":
            prevWifeId = line.split()[-1]
        elif line.split()[1] == "CHIL":
            childInfo[prevHusbId].append(line.split()[-1])
            childInfo[prevWifeId].append(line.split()[-1])

with open("romanovs.pl", "w", encoding="UTF-8") as f:
    for persId in humanInfo:
        if humanInfo[persId]["sex"] == "M":
            name = humanInfo[persId]["name"]
            f.write(f"male(\"{name}\")" + ".\n")
    for persId in humanInfo:
        if humanInfo[persId]["sex"] == "F":
            name = humanInfo[persId]["name"]
            f.write(f"female(\"{name}\")" + ".\n")
    for parentId in childInfo:
        for childId in childInfo[parentId]:
            parentName = humanInfo[parentId]["name"]
            childName = humanInfo[childId]["name"]
            f.write(f"child(\"{childName}\", \"{parentName}\")" + ".\n")