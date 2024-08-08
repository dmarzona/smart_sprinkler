const char* home = R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Smart sprinkler</title>
    <style>
        body {
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
            min-height: 100vh;
            margin: 0;
            background-color: #f0f0f0;
            font-family: Arial, sans-serif;
        }
        .container {
            display: flex;
            gap: 20px;
            margin-bottom: 20px;
        }
        .box {
            background-color: white;
            border-radius: 8px;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
            padding: 20px;
            text-align: center;
            width: 150px;
        }
        .title {
            font-size: 18px;
            font-weight: bold;
            margin-bottom: 10px;
        }
        .quantity {
            font-size: 24px;
            color: #333;
        }
        .large-box {
            background-color: white;
            border-radius: 8px;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
            padding: 20px;
            text-align: center;
            width: 580px; /* 3 times the width of a single box plus two gaps (20px each) */
            margin-bottom: 20px;
        }
        .string {
            font-size: 16px;
            color: #555;
            margin: 5px 0;
        }
        .link {
            font-size: 16px;
            color: #0066cc;
            text-decoration: none;
        }
        .link:hover {
            text-decoration: underline;
        }
    </style>
</head>
<body>
    <h1>Smart sprinkler</h1>
    <div class="container">
        <div class="box">
            <div class="title">Temperature</div>
            <div class="quantity">{{temperature}}°C</div>
        </div>
        <div class="box">
            <div class="title">Pressure</div>
            <div class="quantity">{{pressure}}Pa</div>
        </div>
        <div class="box">
            <div class="title">Current sensor</div>
            <div class="quantity">{{current_sense}}V</div>
        </div>
    </div>
    <div class="large-box">
        <div class="title">Last log lines</div>
        <div class="string">{{log_line_1}}</div>
        <div class="string">{{log_line_2}}</div>
        <div class="string">{{log_line_3}}</div>
        <div class="string">{{log_line_4}}</div>
        <div class="string">{{log_line_5}}</div>
    </div>
    <a class="link" href="/settings.html">Settings</a>
</body>
</html>

)";

const char* settings = R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Settings</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f0f0f0;
            margin: 0;
            height: 100vh;
            display: flex;
            justify-content: center;
            align-items: flex-start; /* Align items to the top center */
        }
        .container {
            margin-top: 50px; /* Add some margin from the top */
            text-align: center;
        }
        h1 {
            color: #333;
        }
        a {
            text-decoration: none;
            color: #007bff;
        }
        a:hover {
            text-decoration: underline;
        }
        input[type="text"] {
            padding: 5px;
            margin-bottom: 10px;
            width: calc(100% - 20px); /* Make input boxes full width minus padding */
            max-width: 300px;
            box-sizing: border-box;
        }
        button {
            padding: 10px 20px; /* Adjust padding for a larger button */
            font-size: 18px; /* Increase font size */
            width: 100px; /* Optional: set a width for uniformity */
            height: 50px; /* Optional: set a height */
            background-color: #007bff; /* Button background color */
            color: white; /* Button text color */
            border: none; /* Remove border */
            border-radius: 5px; /* Rounded corners */
            cursor: pointer; /* Change cursor on hover */
        }
        button:hover {
            background-color: #0056b3; /* Darker background on hover */
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Settings</h1>
        <form action="/settings.html" method="POST">
            <label for="activationTime">Activation time [s]:</label>
            <input type="text" id="activationTime" name="activationTime" value="0"><br>
            <label for="pumpPower">Pump power [%]:</label>
            <input type="text" id="pumpPower" name="pumpPower" value="0"><br>
            <button type="submit">Submit</button>
        </form>
        <a class="link" href="/">Home</a>
    </div>
</body>
</html>
)";