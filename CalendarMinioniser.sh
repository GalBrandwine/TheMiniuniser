curl -d "client_id=878532041560-9ja27q8s2sbej92fm1e5k0k9ekenna3p.apps.googleusercontent.com&client_secret=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& \
         device_code=device_code& \
         grant_type=urn%3Aietf%3Aparams%3Aoauth%3Agrant-type%3Adevice_code" \
         -H "Content-Type: application/json" \
         #-d "GET https://oauth2.googleapis.com/device/code"
         -d "GET https://www.googleapis.com/calendar/v3/users/me/calendarList/gbrandwine@augury.com"
        


 curl -d "client_id=878532041560-9ja27q8s2sbej92fm1e5k0k9ekenna3p.apps.googleusercontent.com&scope=profile" \                                              gal@gal-T14
     https://oauth2.googleapis.com/device/code
{
  "device_code": "AH-1Ng2remBY5aYgcKcDyMQqOsnT3mgaKfr4bMRBqbBj-yPU3L3oktlVtg8jbdPZ-Pq_40DaObrHLgr1WE73o7abPW0JKhYfIQ",
  "user_code": "BLD-SPH-DVD",
  "expires_in": 1800,
  "interval": 5,
  "verification_url": "https://www.google.com/device"
}%      

# curl -H "Authorization: Bearer access_token" https://www.googleapis.com/drive/v2/files    


# Get all events with my user in them that start between timeMin and timeMax
curl \                                                                                                                                                  gal@gal-T14
  'https://www.googleapis.com/calendar/v3/calendars/gbrandwine%40augury.com/events?q=gbrandwine&timeMax=2022-01-24T00%3A00%3A00%2B02%3A00&timeMin=2022-01-23T00%3A00%3A00%2B02%3A00&fields=items' \
  --header 'Authorization: Bearer ya29.A0ARrdaM-vG3HeznbyrB4-ZUZ0YXQVGWeS5Tbp8c34k06AEj2wQu12IIEszIajvyjUntUP8oXixhBunW7PwmXPLQMePk-pXx49ZBJgGYJ9QAD0z_Qwc8N2xNUsl52_BTz8-9Z7Rj3B06FTvhiOG13bbA5iWB7Q9g' \
  --header 'Accept: application/json' \
  --compressed
