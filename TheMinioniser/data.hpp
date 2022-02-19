#pragma once

namespace token_data
{

    // This token lives for 3600 seconds
    String token = "";
    // Given while requesting Oauth2.0 token for the first time.
    // Using this token we can refresh the original token expiration time
    String refresh_token = "";
    // This is constant with the google API
    String client_secret = "";
    String client_id = "";
    String refreshTokenUtl = "";

    int token_expiration_time = 0;
} // token_data