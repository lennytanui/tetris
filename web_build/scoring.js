
function SaveScore(score){
    fetch("http://localhost:3000/scores", {
        method: "POST",
        headers: {
            "Content-Type": "application/json",
        },
        body: JSON.stringify({ username: JSON.stringify(score)}),
    }).then(response => {
        return response.json()
    }).then(value => {
        console.log("Got something from server: ", value);
    }).catch(err => {
        console.error("Error Saving Score: ", err);
    })
}

// GetScores();
async function GetScores(){
    let result = {};
    await fetch("http://localhost:3000/scores", {
        method: "GET",
        headers: {
            "Content-Type": "application/json",
        }
    }).then(response => {
        return response.json()
    }).then(scores => {
        result.username += '\0';
        result = scores;
    }).catch(err => {
        console.error("Error Saving Score: ", err);
    })

    console.log("Scores : ", result);
    
    let testResult = {
        "id" : 123,
        "username" : "test username",
        "score" : 321,
        "date" : "01/01/0001",
        "time" : "11:11"
    };

    console.log("Test Results ", testResult);
    _GetLeaderBoard(testResult);
    return result;
}