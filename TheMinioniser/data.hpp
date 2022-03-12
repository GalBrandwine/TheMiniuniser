#pragma once

namespace token_data
{

    // This token lives for 3600 seconds
    String token = "";
    // Given while requesting Oauth2.0 token for the first time.
    // Using this token we can refresh the original token expiration time
    String refresh_token = "";
    // This is constant with the google API
    const String client_secret = "";
    const String client_id = "";
    String refreshTokenUtl = "https://oauth2.googleapis.com/token";
    String USER_NAME = "gbrandwine";
    String USER_DOMAIN = "@augury.com";
    int token_expiration_time = 0;
} // token_data