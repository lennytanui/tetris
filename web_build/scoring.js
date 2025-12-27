
function SaveScore(score){
    fetch("/scores", {
        method: "POST",
        headers: {
            "Content-Type": "application/json",
        },
        body: JSON.stringify(score),
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
    let result = [];
    await fetch("/scores", {
        method: "GET",
        headers: {
            "Content-Type": "application/json",
        }
    }).then(response => {
        return response.json();
    }).then(scores => {
        result = scores;
    }).catch(err => {
        console.error("Error Saving Score: ", err);
    })
    
    return result;
}