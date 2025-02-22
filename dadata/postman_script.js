//postman script for http://suggestions.dadata.ru/suggestions/api/4_1/rs/geolocate/postal_unit

let responseData = pm.response.json(); // Parse JSON response

// Get the current day of the week (0 = Sunday, 1 = Monday, ..., 6 = Saturday)
let days = ["sun", "mon", "tue", "wed", "thu", "fri", "sat"];
let currentDay = days[new Date().getDay()]; // Example: "mon"

// Get current time in HH:mm format
let now = new Date();
let currentTime = `${String(now.getHours()).padStart(2, '0')}:${String(now.getMinutes()).padStart(2, '0')}`;

console.log(`Current time: ${currentTime}, Checking schedule for: ${currentDay}`);

if (responseData.suggestions && responseData.suggestions.length > 0) {
    console.log("Filtered Open Postal Offices:");

    responseData.suggestions.forEach((suggestion, index) => {
        if (suggestion.data) {
            let scheduleKey = `schedule_${currentDay}`;
            let schedule = suggestion.data[scheduleKey];

            if (schedule) {
                let [openTime, closeTime] = schedule.split("-");

                // Check if the current time is within open hours
                if (currentTime >= openTime && currentTime <= closeTime) {
                    console.log(`Suggestion ${index + 1} (Open):`);
                    console.log(`  Address: ${suggestion.data.address_str}`);
                    console.log(`  Type: ${suggestion.data.type_code}`);
                    console.log(`  Postal Code: ${suggestion.data.postal_code}`);
                    console.log(`  Open Hours Today: ${schedule}`);
                    console.log("-----------------\n");
                }
            }
        }
    });
} else {
    console.log("No open postal offices found.");
}
