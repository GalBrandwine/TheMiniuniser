#pragma once

namespace token_data
{

    // This token lives for 3600 seconds
    String token = "ya29.A0ARrdaM9GuL-ryFtNmFE5LeSnmaoF-EjtC_WCloNLYBOMz1S-hIOqcT9LYvU3-9d_KZfED-J0xfb7A1yLkHGp8iz51E8gaeOz50ZDlbiem04QoRxZp-aAT81qE-pcy6hw7Zxe5R2Lxso2d9sBBQtSc5p6E6JZHA";
    // Given while requesting Oauth2.0 token for the first time.
    // Using this token we can refresh the original token expiration time
    String refresh_token = "1%2F%2F09FGDh7P0XsC7CgYIARAAGAkSNwF-L9Irp5l5BNGBf9ZjfZ_eUdP13aMALPKWmH71tSiBx7wVzyU3C_rEr6MabhoM-xJ787av5TU";
    // This is constant with the google API
    String client_secret = "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";
    String client_id = "878532041560-9ja27q8s2sbej92fm1e5k0k9ekenna3p.apps.googleusercontent.com";
    String refreshTokenUtl = "https://oauth2.googleapis.com/token";
    const String USER_NAME = "gbrandwine";
    const String USER_DOMAIN = "@augury.com";
    int token_expiration_time = 0;
} // token_data