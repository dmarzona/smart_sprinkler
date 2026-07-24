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
            display: block;
            margin-top: 20px;
            font-size: 18px;
            text-decoration: none;
            color: #007bff;
        }
        .link:hover {
            color: #0056b3;
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
            <div class="title">Humidity</div>
            <div class="quantity">{{pressure}}%</div>
        </div>
        <div class="box">
            <div class="title">Current sensor</div>
            <div class="quantity">{{current_sense}}V</div>
        </div>
    </div>
    <form action="/" method="POST">
        <button type="submit" name="pump" value="0">
            Activate pump 1
        </button>

        <button type="submit" name="pump" value="1">
            Activate pump 2
        </button>
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
        .link {
            display: block;
            margin-top: 20px;
            font-size: 18px;
            text-decoration: none;
            color: #007bff;
        }
        .link:hover {
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

                <table width="100%">
                    <tr>
                        <!-- LEFT SIDE -->
                        <td width="50%" valign="top">

                            <h3>Pump 1</h3>

                            <label for="activation_time1">
                                Pump 1 activation time [s]:
                            </label><br>

                            <input type="text"
                                id="activation_time1"
                                name="activation_time1"
                                value="{{activation_time0}}"><br><br>

                            <label for="pump_power1">
                                Pump 1 power [%]:
                            </label><br>

                            <input type="text"
                                id="pump_power1"
                                name="pump_power1"
                                value="{{pump_power0}}"><br>

                        </td>

                        <!-- RIGHT SIDE -->
                        <td width="50%" valign="top">

                            <h3>Pump 2</h3>

                            <label for="activation_time2">
                                Pump 2 activation time [s]:
                            </label><br>

                            <input type="text"
                                id="activation_time2"
                                name="activation_time2"
                                value="{{activation_time1}}"><br><br>

                            <label for="pump_power2">
                                Pump 2 power [%]:
                            </label><br>

                            <input type="text"
                                id="pump_power2"
                                name="pump_power2"
                                value="{{pump_power1}}"><br>

                        </td>
                    </tr>

                    <!-- BUTTON CENTERED -->
                    <tr>
                        <td colspan="2" align="center">
                            <br>
                            <button type="submit">
                                Update Pump Settings
                            </button>
                        </td>
                    </tr>

                </table>

            </form>
        </div>

        <!-- Dynamic Entries Column -->
        <div class="column">
            <h2>Irrigation start times pump 1</h2>

            <form action="/irrigation_start_times"
                method="POST"
                id="dynamicForm1">

                <input type="hidden" name="pump_id" value="1">

                <div id="entries1">
                    {{additionalEntries0}}
                </div>

                <button type="button" onclick="addEntry('entries1')">
                    Add Entry
                </button>

                <button type="submit"
                        onclick="return validateForm('entries1')">
                    Submit All Entries
                </button>

            </form>
        </div>


        <!-- SECOND DIV BELOW THE FIRST -->
        <div class="column">
            <h2>Irrigation start times pump 2</h2>

            <form action="/irrigation_start_times"
                method="POST"
                id="dynamicForm2">

                <input type="hidden" name="pump_id" value="2">

                <div id="entries2">
                    {{additionalEntries1}}
                </div>

                <button type="button" onclick="addEntry('entries2')">
                    Add Entry
                </button>

                <button type="submit"
                        onclick="return validateForm('entries2')">
                    Submit All Entries
                </button>

            </form>
        </div>
    </div>
    <!-- Centered Home Link -->
    <a href="/" class="link">Home</a>
    <!-- Centered Update Link -->
    <a href="/updatePage" class="link">Firmware Update</a>
    <script>
        let entryCount1 = {{entryCount0}};
        let entryCount2 = {{entryCount1}};

        // Add entry to selected container
        function addEntry(entriesId) {
            let currentCount;
            if (entriesId === 'entries1') {
                currentCount = entryCount1;
            } else {
                currentCount = entryCount2;
            }

            if (currentCount < 10) {
                currentCount++;
                if (entriesId === 'entries1') {
                    entryCount1 = currentCount;
                } else {
                    entryCount2 = currentCount;
                }

                const entryDiv = document.getElementById(entriesId);
                const newEntry = document.createElement('div');

                newEntry.className = 'entry-group';
                newEntry.innerHTML = `
                    <input type="text"
                        id="entry${currentCount}"
                        name="entry${currentCount}"
                        value="00:00:00"
                        placeholder="HH:MM:SS">

                    <button type="button"
                            class="remove-btn"
                            onclick="removeEntry(this, '${entriesId}')">
                        Remove
                    </button>
                `;

                entryDiv.appendChild(newEntry);
            } else {
                alert('Maximum of 10 entries reached');
            }
        }

        // Remove entry
        function removeEntry(button, entriesId) {

            const entryGroup = button.parentElement;

            entryGroup.remove();

            if (entriesId === 'entries1') {
                entryCount1--;
            } else {
                entryCount2--;
            }
        }

        // Validate selected form
        function validateForm(entriesId) {

            const entries =
                document.querySelectorAll(
                    '#' + entriesId + ' input[type="text"]'
                );

            const timePattern =
                /^([0-1]\d|2[0-3]):([0-5]\d):([0-5]\d)$/;

            for (let i = 0; i < entries.length; i++) {

                if (!timePattern.test(entries[i].value)) {

                    alert(
                        'Invalid time format in entry '
                        + (i + 1)
                        + ', is '
                        + entries[i].value
                        + '. Please use HH:MM:SS.'
                    );

                    return false;
                }
            }

            return true;
        }
    </script>
</body>
</html>
)delim";

const char* updatePage = R"delim(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Firmware Update</title>

    <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js"></script>

    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f0f0f0;
            margin: 20px;
            text-align: center;
        }

        h1 {
            color: #333;
            margin-bottom: 20px;
        }

        .container {
            display: flex;
            justify-content: center;
        }

        .column {
            width: 500px;
            padding: 20px;
            background-color: #fff;
            margin: 10px;
            border-radius: 8px;
            box-shadow: 0 2px 4px rgba(0,0,0,0.1);
            text-align: left;
        }

        input[type="file"] {
            width: 100%;
            margin: 15px 0;
        }

        button {
            padding: 10px 20px;
            font-size: 16px;
            background-color: #007bff;
            color: white;
            border: none;
            border-radius: 5px;
            cursor: pointer;
        }

        button:hover {
            background-color: #0056b3;
        }

        .progress-container {
            width: 100%;
            background-color: #ddd;
            border-radius: 5px;
            margin-top: 20px;
            overflow: hidden;
        }

        .progress-bar {
            width: 0%;
            height: 25px;
            background-color: #28a745;
            text-align: center;
            line-height: 25px;
            color: white;
            transition: width 0.2s;
        }

        #status {
            margin-top: 15px;
            font-weight: bold;
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
    </style>
</head>

<body>

<h1>Firmware Update</h1>

<div class="container">
    <div class="column">

        <h2>Select Firmware File</h2>

        <form method="POST"
              action="#"
              enctype="multipart/form-data"
              id="upload_form">

            <input type="file"
                   name="update"
                   accept=".bin"
                   required>

            <br>

            <button type="submit">
                Upload Firmware
            </button>

        </form>

        <div class="progress-container">
            <div class="progress-bar" id="progressBar">
                0%
            </div>
        </div>

        <div id="status">
            Waiting for upload...
        </div>

    </div>
</div>

<a href="/" class="home-link">Home</a>
<a href="/settings" class="home-link">Settings</a>

<script>

$("#upload_form").submit(function(e){

    e.preventDefault();

    var form = $("#upload_form")[0];
    var data = new FormData(form);

    $.ajax({

        url: "/update",
        type: "POST",
        data: data,
        contentType: false,
        processData: false,

        xhr: function() {

            var xhr = new window.XMLHttpRequest();

            xhr.upload.addEventListener("progress", function(evt){

                if(evt.lengthComputable){

                    var percent = Math.round((evt.loaded / evt.total) * 100);

                    $("#progressBar")
                        .css("width", percent + "%")
                        .text(percent + "%");

                    $("#status").text("Uploading...");

                }

            }, false);

            return xhr;
        },

        success: function(){

            $("#status").text("Update complete. Device is rebooting...");

        },

        error: function(){

            $("#status").text("Upload failed.");

        }

    });

});

</script>

</body>
</html>
)delim";
