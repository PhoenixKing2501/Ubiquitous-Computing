# Define an array with the words
$words = @("good", "regular", "bad")

# Start an infinite loop
while ($true) {
	# Select a random word from the array
	$randomWord = Get-Random -InputObject $words

	# Publish the word to the MQTT broker
	mosquitto_pub -t 'picture' -m $randomWord -d

	# Wait for 10 seconds
	Start-Sleep -Seconds 10
}
