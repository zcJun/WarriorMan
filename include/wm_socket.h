/**
 * 对于socket进行封装
 */
#ifndef _WM_SOCKET_H
#define _WM_SOCKET_H

#include "header.h"
#include "coroutine.h"
#include "wm_string.h"
#include "timer.h"

typedef void (*wm_socket_func_t)(void*);

typedef struct {
	int fd; //文件描述符
	wmString *write_buffer; //写缓冲区
	int maxSendBufferSize; //应用层发送缓冲区
	int events; //loop监听了什么事件
	bool closed; //连接是否关闭
	bool removed; //连接是否close
	void* owner; //拥有人，比如connection创建的socket，owner就是这个connection
	int errCode; //错误码
	const char *errMsg; //错误描述

	wmCoroutine* read_co; //read操作的时候，保存当前的协程，以便正确唤醒
	wmCoroutine* write_co; //write操作的时候，保存当前的，以便正确唤醒

	/**
	 * worker和connection类型会自己管理loop。runtime是在read或者write的时候代为管理
	 */
	int loop_type; //对应wmLoop_type这个枚举
	int transport; //什么协议类型，比如TCP UDP等
	wm_socket_func_t onBufferWillFull;
	char* connect_host;
	int connect_port;

	/**
	 * runtime用
	 */
	bool shutdown_read;
	bool shutdown_write;

	/**
	 * 判断操作是否超时
	 */
	wmTimerWheel_Node* read_timer;
	wmTimerWheel_Node* write_timer;

	uint32_t read_timeout; //读超时默认时间
} wmSocket;

wmSocket* wmSocket_create(int transport, int loop_type);
wmSocket* wmSocket_pack(int fd, int transport, int loop_type);
int wmSocket_read(wmSocket* socket, char *buf, int len, uint32_t timeout);
int wmSocket_send(wmSocket *socket, const void *buf, size_t len);
int wmSocket_write(wmSocket *socket, const void *buf, size_t len); //不管缓冲区
int wmSocket_close(wmSocket *socket);
void wmSocket_free(wmSocket *socket);
bool wmSocket_connect(wmSocket *socket, char* _host, int _port, uint32_t timeout);
wmSocket * wmSocket_accept(wmSocket* socket, int new_socket_loop_type, uint32_t timeout);
ssize_t wmSocket_peek(wmSocket* socket, void *__buf, size_t __n);
bool wmSocket_shutdown(wmSocket *socket, int __how);
ssize_t wmSocket_recvfrom(wmSocket* socket, void *__buf, size_t __n, struct sockaddr* _addr, socklen_t *_socklen, uint32_t timeout);
#endif
