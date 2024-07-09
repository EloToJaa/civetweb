/*
 * Copyright (c) 2018 the CivetWeb developers
 * Revisited version: Copyright (c) 2022 the CivetWeb developers
 * MIT License
 */

/* Simple demo of a REST callback. */
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "cJSON.h"
#include "civetweb.h"

#define PORT "8089"
#define HOST_INFO "http://localhost:8089"

#define EXAMPLE_URI "/test"

int exitNow = 0;

static int SendJSON(struct mg_connection *conn, cJSON *json_obj)
{
	char *json_str = cJSON_PrintUnformatted(json_obj);
	size_t json_str_len = strlen(json_str);

	/* Send HTTP message header (+1 for \n) */
	mg_send_http_ok(conn, "application/json; charset=utf-8", json_str_len + 1);

	/* Send HTTP message content */
	mg_write(conn, json_str, json_str_len);

	/* Add a newline. This is not required, but the result is more
	 * human-readable in a debugger. */
	mg_write(conn, "\n", 1);

	/* Free string allocated by cJSON_Print* */
	cJSON_free(json_str);

	return (int)json_str_len;
}

static int TestHandler(struct mg_connection *conn, void *cbdata)
{
	cJSON *obj = cJSON_CreateObject();

	if (!obj)
	{
		/* insufficient memory? */
		mg_send_http_error(conn, 500, "Server error");
		return 500;
	}

	cJSON_AddStringToObject(obj, "version", CIVETWEB_VERSION);
	SendJSON(conn, obj);
	cJSON_Delete(obj);

	return 200;
}

static int log_message(const struct mg_connection *conn, const char *message)
{
	puts(message);
	return 1;
}

int main(int argc, char *argv[])
{
	const char *options[] = {
		"listening_ports",
		PORT,
		"request_timeout_ms",
		"10000",
		"error_log_file",
		"error.log",
		0};

	struct mg_callbacks callbacks;
	struct mg_context *ctx;
	int err = 0;

	/* Init libcivetweb. */
	mg_init_library(0);

	/* Callback will print error messages to console */
	memset(&callbacks, 0, sizeof(callbacks));
	callbacks.log_message = log_message;

	/* Start CivetWeb web server */
	ctx = mg_start(&callbacks, 0, options);

	/* Check return value: */
	if (ctx == NULL)
	{
		fprintf(stderr, "Cannot start CivetWeb - mg_start failed.\n");
		return 1;
	}

	/* Add handler EXAMPLE_URI, to explain the example */
	mg_set_request_handler(ctx, EXAMPLE_URI, TestHandler, 0);

	/* Show some info */
	printf("Start example: %s%s\n", HOST_INFO, EXAMPLE_URI);

	/* Wait until the server should be closed */
	while (!exitNow)
	{
#ifdef _WIN32
		Sleep(1000);
#else
		sleep(1);
#endif
	}

	/* Stop the server */
	mg_stop(ctx);

	printf("Server stopped.\n");

	return 0;
}
