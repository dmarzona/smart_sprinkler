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
        button {
            padding: 10px 20px; /* Adjust padding for a larger button */
            font-size: 18px; /* Increase font size */
            background-color: #007bff; /* Button background color */
            color: white; /* Button text color */
            border: none; /* Remove border */
            border-radius: 5px; /* Rounded corners */
            cursor: pointer; /* Change cursor on hover */
            margin-bottom: 20px;
        }
        button:hover {
            background-color: #0056b3; /* Darker background on hover */
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
    <form action="/" method="POST">
        <button type="submit">Activate pump</button>
    </form>
    <a class="link" href="/settings">Settings</a>
</body>
</html>

)";

const char* settings = R"delim(
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
            margin: 20px;
            text-align: center; /* Center align the text */
        }
        h1 {
            color: #333;
            margin-bottom: 20px;
        }
        .home-link {
            display: block;
            margin-top: 20px;
            font-size: 18px;
            text-decoration: none;
            color: #007bff;
        }
        .home-link:hover {
            color: #0056b3;
            text-decoration: underline;
        }
        .container {
            display: flex;
            justify-content: space-between;
        }
        .column {
            flex: 1;
            padding: 20px;
            background-color: #fff;
            margin: 10px;
            border-radius: 8px;
            box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
            text-align: left; /* Align the content inside columns to the left */
        }
        input[type="text"] {
            padding: 5px;
            margin-bottom: 10px;
            width: calc(100% - 70px); /* Adjust width to fit the remove button */
            box-sizing: border-box;
        }
        button {
            padding: 10px 20px;
            font-size: 16px;
            background-color: #007bff;
            color: white;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            margin-top: 10px;
        }
        button:hover {
            background-color: #0056b3;
        }
        .remove-btn {
            padding: 5px;
            font-size: 14px;
            background-color: #dc3545;
            color: white;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            margin-left: 5px;
        }
        .remove-btn:hover {
            background-color: #c82333;
        }
        .entry-group {
            display: flex;
            align-items: center;
            margin-bottom: 10px;
        }
    </style>
</head>
<body>
    <h1>Settings</h1>
    <div class="container">
        <!-- Pump Settings Column -->
        <div class="column">
            <h2>Pump Settings</h2>
            <form action="/pump_settings" method="POST">
                <label for="activation_time">Pump activation time [s]:</label>
                <input type="text" id="activation_time" name="activation_time" value="{{activation_time}}"><br>
                <label for="pump_power">Pump power [%]:</label>
                <input type="text" id="pump_power" name="pump_power" value="{{pump_power}}"><br>
                <button type="submit">Update Pump Settings</button>
            </form>
        </div>

        <!-- Dynamic Entries Column -->
        <div class="column">
            <h2>Irrigation start times</h2>
            <form action="/irrigation_start_times" method="POST" id="dynamicForm">
                <div id="entries">
                    {{additionalEntries}}
                </div>
                <button type="button" onclick="addEntry()">Add Entry</button>
                <button type="submit" onclick="return validateForm()">Submit All Entries</button>
            </form>
        </div>
    </div>
    <!-- Centered Home Link -->
    <a href="/" class="home-link">Home</a>
    <script>
        let entryCount = {{entryCount}};

        // Function to add a new entry
        function addEntry() {
            if (entryCount < 10) {
                entryCount++;
                const entryDiv = document.getElementById('entries');
                const newEntry = document.createElement('div');
                newEntry.className = 'entry-group';
                newEntry.innerHTML = `
                    <input type="text" id="entry${entryCount}" name="entry${entryCount}" value="00:00:00" placeholder="HH:MM:SS">
                    <button type="button" class="remove-btn" onclick="removeEntry(this)">Remove</button>
                `;
                entryDiv.appendChild(newEntry);
            } else {
                alert('Maximum of 10 entries reached');
            }
        }

        // Function to remove a specific entry
        function removeEntry(button) {
            const entryGroup = button.parentElement;
            entryGroup.remove();
            entryCount--;
        }

        // Function to validate the time format HH:MM:SS
        function validateForm() {
            const entries = document.querySelectorAll('#entries input[type="text"]');
            const timePattern = /^([0-1]\d|2[0-3]):([0-5]\d):([0-5]\d)$/; // Regex for HH:MM:SS

            for (let i = 0; i < entries.length; i++) {
                if (!timePattern.test(entries[i].value)) {
                    alert('Invalid time format in entry ' + (i + 1) + ', is '+ entries[i].value +   '. Please use HH:MM:SS.');
                    return false; // Prevent form submission
                }
            }

            return true; // Allow form submission if all entries are valid
        }
    </script>
</body>
</html>
)delim";
