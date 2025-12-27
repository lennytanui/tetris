import os
import shutil
from time import sleep

dirpath = os.path.dirname(os.path.realpath(__file__))

fileModifications = [
    {"filename" : "main.data", "timeModified" : ""},
    {"filename" : "main.wasm", "timeModified" : ""},
    {"filename" : "main.js", "timeModified" : ""}, 
    {"filename" : "scoring.js", "timeModified" : ""}, 
]

def watchFile():
    global dirpath
    global fileModifications
    for index, f in enumerate(fileModifications):
        filename = f["filename"]
        timeModified = f["timeModified"]
        filepath = dirpath + "\\"  + filename
        currentModifiedTime = os.path.getmtime(filepath)

        if(currentModifiedTime != timeModified):
            print(filepath, " has been modified")
            fileModifications[index]["timeModified"] = currentModifiedTime
            newDestination = "c:\\dev\\lennyscanvas\\public\\"

            extension = os.path.splitext(filepath)[1]

            if extension == ".wasm":
                newDestination += "wasm\\"
            elif extension == ".txt":
                newDestination += "txt\\"
            elif extension == ".js":
                newDestination += "javascripts\\"

            newDestination += filename
            shutil.copy2(filepath, newDestination)

while True:
    watchFile()
    sleep(0.5)