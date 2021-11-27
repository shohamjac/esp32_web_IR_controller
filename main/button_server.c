/* HTTP File Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <dirent.h>

#include "esp_err.h"
#include "esp_log.h"

#include "esp_vfs.h"
#include "esp_spiffs.h"
#include "esp_http_server.h"
#include "ledc.h"
#include "i2s_dac.h"
#include "rmt.h"

/* Max length a file path can have on storage */
#define FILE_PATH_MAX (ESP_VFS_PATH_MAX + CONFIG_SPIFFS_OBJ_NAME_LEN)

/* Max size of an individual file. Make sure this
 * value is same as that set in upload_script.html */
#define RMT_WAIT_TIME_MS 40

/* Scratch buffer size */
#define SCRATCH_BUFSIZE  8192

struct file_server_data {
    /* Base path of file storage */
    char base_path[ESP_VFS_PATH_MAX + 1];

    /* Scratch buffer for temporary storage during file transfer */
    char scratch[SCRATCH_BUFSIZE];
};

static const char *TAG = "file_server";

/* Handler to redirect incoming GET request for /index.html to / */
static esp_err_t index_html_get_handler(httpd_req_t *req)
{
    httpd_resp_set_status(req, "301 Permanent Redirect");
    httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_send(req, NULL, 0);  // Response body can be empty
    return ESP_OK;
}

/* Send HTTP response with a run-time generated html consisting of
 * a list of all files and folders under the requested path */
static esp_err_t http_resp_dir_html(httpd_req_t *req)
{
    /* Get handle to embedded file upload script */
    extern const unsigned char upload_script_start[] asm("_binary_upload_script_html_start");
    extern const unsigned char upload_script_end[]   asm("_binary_upload_script_html_end");
    const size_t upload_script_size = (upload_script_end - upload_script_start);

    /* Add file upload form and script which on execution sends a POST request to /upload */
    httpd_resp_send_chunk(req, (const char *)upload_script_start, upload_script_size);


    /* Send empty chunk to signal HTTP response completion */
    httpd_resp_sendstr_chunk(req, NULL);
    return ESP_OK;
}



/* Handler to download a file kept on the server */
static esp_err_t download_get_handler(httpd_req_t *req)
{
    // Check if the target is a directory
    if (req->uri[strlen(req->uri) - 1] == '/') {
        // In so, send an html with directory listing
        http_resp_dir_html(req);
    } else {
        // Else send the file
    }
    return ESP_OK;
}

/* Handler to shout, AKA turn the LED on and off */
static esp_err_t power_post_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "power!");
    //fade_up();
    //fade_down();

    rmt_sony_power();

    vTaskDelay(RMT_WAIT_TIME_MS / portTICK_PERIOD_MS);

    rmt_sony_power();

    vTaskDelay(RMT_WAIT_TIME_MS / portTICK_PERIOD_MS);

    rmt_sony_power();

    /* Redirect onto root to see the updated file list */
//  httpd_resp_set_status(req, "303 See Other");
//  httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_sendstr(req, "shouted");
    return ESP_OK;
}

/* Handler to volume up */
static esp_err_t v_up_post_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "volume up!");

    rmt_sony_v_up();

    vTaskDelay(RMT_WAIT_TIME_MS / portTICK_PERIOD_MS);

    rmt_sony_v_up();

    vTaskDelay(RMT_WAIT_TIME_MS / portTICK_PERIOD_MS);

    rmt_sony_v_up();

    /* Redirect onto root to see the updated file list */
//  httpd_resp_set_status(req, "303 See Other");
//  httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_sendstr(req, "volume uped");
    return ESP_OK;
}

/* Handler to volume down */
static esp_err_t v_down_post_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "volume down!");

    rmt_sony_v_down();

    vTaskDelay(RMT_WAIT_TIME_MS / portTICK_PERIOD_MS);

    rmt_sony_v_down();

    vTaskDelay(RMT_WAIT_TIME_MS / portTICK_PERIOD_MS);

    rmt_sony_v_down();

    /* Redirect onto root to see the updated file list */
//  httpd_resp_set_status(req, "303 See Other");
//  httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_sendstr(req, "volume downed");
    return ESP_OK;
}

/* Handler to volume down */
static esp_err_t input_post_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "input!");

    rmt_sony_input();

    vTaskDelay(RMT_WAIT_TIME_MS / portTICK_PERIOD_MS);

    rmt_sony_input();

    vTaskDelay(RMT_WAIT_TIME_MS / portTICK_PERIOD_MS);

    rmt_sony_input();

    /* Redirect onto root to see the updated file list */
//  httpd_resp_set_status(req, "303 See Other");
//  httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_sendstr(req, "input");
    return ESP_OK;
}


/* Handler to up button */
static esp_err_t up_post_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "up");

    rmt_sony_up();

    vTaskDelay(RMT_WAIT_TIME_MS / portTICK_PERIOD_MS);

    rmt_sony_up();

    vTaskDelay(RMT_WAIT_TIME_MS / portTICK_PERIOD_MS);

    rmt_sony_up();

    /* Redirect onto root to see the updated file list */
//  httpd_resp_set_status(req, "303 See Other");
//  httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_sendstr(req, "uped");
    return ESP_OK;
}

/* Handler to down button */
static esp_err_t down_post_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "down");

    rmt_sony_down();

    vTaskDelay(RMT_WAIT_TIME_MS / portTICK_PERIOD_MS);

    rmt_sony_down();

    vTaskDelay(RMT_WAIT_TIME_MS / portTICK_PERIOD_MS);

    rmt_sony_down();

    /* Redirect onto root to see the updated file list */
//  httpd_resp_set_status(req, "303 See Other");
//  httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_sendstr(req, "downed");
    return ESP_OK;
}

/* Handler to ok button */
static esp_err_t ok_post_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "ok");

    rmt_sony_ok();

    vTaskDelay(RMT_WAIT_TIME_MS / portTICK_PERIOD_MS);

    rmt_sony_ok();

    vTaskDelay(RMT_WAIT_TIME_MS / portTICK_PERIOD_MS);

    rmt_sony_ok();

    /* Redirect onto root to see the updated file list */
//  httpd_resp_set_status(req, "303 See Other");
//  httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_sendstr(req, "ok-ed");
    return ESP_OK;
}


/* Handler to sing, AKA play music */
static esp_err_t sing_post_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "sing!");
    ESP_LOGI(TAG, "uri: %s", req->uri);
    
    if(strcmp(req->uri, "/sing") == 0) {
        sing_i2s_dac(1);    
    }
    else {
        sing_i2s_dac(2);
    }

    httpd_resp_sendstr(req, "sang");
    return ESP_OK;
}

/* Handler to respond with an icon file embedded in flash.
 * Browsers expect to GET website icon at URI /favicon.ico */
static esp_err_t favicon_get_handler(httpd_req_t *req)
{
    extern const unsigned char favicon_ico_start[] asm("_binary_favicon_ico_start");
    extern const unsigned char favicon_ico_end[]   asm("_binary_favicon_ico_end");
    const size_t favicon_ico_size = (favicon_ico_end - favicon_ico_start);
    httpd_resp_set_type(req, "image/x-icon");
    httpd_resp_send(req, (const char *)favicon_ico_start, favicon_ico_size);
    return ESP_OK;
}

/* Function to start the file server */
esp_err_t start_file_server(const char *base_path)
{
    static struct file_server_data *server_data = NULL;

    /* Validate file storage base path */
    if (!base_path || strcmp(base_path, "/spiffs") != 0) {
        ESP_LOGE(TAG, "File server presently supports only '/spiffs' as base path");
        return ESP_ERR_INVALID_ARG;
    }

    if (server_data) {
        ESP_LOGE(TAG, "File server already started");
        return ESP_ERR_INVALID_STATE;
    }

    /* Allocate memory for server data */
    server_data = calloc(1, sizeof(struct file_server_data));
    if (!server_data) {
        ESP_LOGE(TAG, "Failed to allocate memory for server data");
        return ESP_ERR_NO_MEM;
    }
    strlcpy(server_data->base_path, base_path,
            sizeof(server_data->base_path));

    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    /* Use the URI wildcard matching function in order to
     * allow the same handler to respond to multiple different
     * target URIs which match the wildcard scheme */
    config.uri_match_fn = httpd_uri_match_wildcard;

    ESP_LOGI(TAG, "Starting HTTP Server");
    if (httpd_start(&server, &config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start file server!");
        return ESP_FAIL;
    }

    /* Register handler for index.html which should redirect to / */
    httpd_uri_t index_html = {
        .uri       = "/index.html",
        .method    = HTTP_GET,
        .handler   = index_html_get_handler,
        .user_ctx  = NULL
    };
    httpd_register_uri_handler(server, &index_html);

    /* Handler for URI used by browsers to get website icon */
    httpd_uri_t favicon_ico = {
        .uri       = "/favicon.ico",
        .method    = HTTP_GET,
        .handler   = favicon_get_handler,
        .user_ctx  = NULL
    };
    httpd_register_uri_handler(server, &favicon_ico);

    /* URI handler for getting uploaded files */
    httpd_uri_t file_download = {
        .uri       = "/*",  // Match all URIs of type /path/to/file (except index.html)
        .method    = HTTP_GET,
        .handler   = download_get_handler,
        .user_ctx  = server_data    // Pass server data as context
    };
    httpd_register_uri_handler(server, &file_download);

    /* URI handler for power */
    httpd_uri_t power = {
        .uri       = "/power",   // Match all URIs of type /upload/path/to/file
        .method    = HTTP_POST,
        .handler   = power_post_handler,
        .user_ctx  = server_data    // Pass server data as context
    };
    httpd_register_uri_handler(server, &power);

    /* URI handler for volume up */
    httpd_uri_t v_up = {
        .uri       = "/v_up",   // Match all URIs of type /upload/path/to/file
        .method    = HTTP_POST,
        .handler   = v_up_post_handler,
        .user_ctx  = server_data    // Pass server data as context
    };
    httpd_register_uri_handler(server, &v_up);

    /* URI handler for volume down */
    httpd_uri_t v_down = {
        .uri       = "/v_down",   // Match all URIs of type /upload/path/to/file
        .method    = HTTP_POST,
        .handler   = v_down_post_handler,
        .user_ctx  = server_data    // Pass server data as context
    };
    httpd_register_uri_handler(server, &v_down);

    /* URI handler for input */
    httpd_uri_t input = {
        .uri       = "/input",   // Match all URIs of type /upload/path/to/file
        .method    = HTTP_POST,
        .handler   = input_post_handler,
        .user_ctx  = server_data    // Pass server data as context
    };
    httpd_register_uri_handler(server, &input);

    /* URI handler for up */
    httpd_uri_t up = {
        .uri       = "/up",   // Match all URIs of type /upload/path/to/file
        .method    = HTTP_POST,
        .handler   = up_post_handler,
        .user_ctx  = server_data    // Pass server data as context
    };
    httpd_register_uri_handler(server, &up);

    /* URI handler for down */
    httpd_uri_t down = {
        .uri       = "/down",   // Match all URIs of type /upload/path/to/file
        .method    = HTTP_POST,
        .handler   = down_post_handler,
        .user_ctx  = server_data    // Pass server data as context
    };
    httpd_register_uri_handler(server, &down);

    /* URI handler for ok */
    httpd_uri_t ok = {
        .uri       = "/ok",   // Match all URIs of type /upload/path/to/file
        .method    = HTTP_POST,
        .handler   = ok_post_handler,
        .user_ctx  = server_data    // Pass server data as context
    };
    httpd_register_uri_handler(server, &ok);


    /* URI handler for singing */
    httpd_uri_t sing = {
        .uri       = "/sing",   // Match all URIs of type /sing
        .method    = HTTP_POST,
        .handler   = sing_post_handler,
        .user_ctx  = server_data    // Pass server data as context
    };
    httpd_register_uri_handler(server, &sing);

    /* URI handler for singing */
    httpd_uri_t sing2 = {
        .uri       = "/sing2",   // Match all URIs of type /sing
        .method    = HTTP_POST,
        .handler   = sing_post_handler,
        .user_ctx  = server_data    // Pass server data as context
    };
    httpd_register_uri_handler(server, &sing2);

    return ESP_OK;
}