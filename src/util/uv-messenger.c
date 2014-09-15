#include <mbgl/util/uv-messenger.h>
#include <mbgl/util/queue.h>

#include <stdlib.h>

typedef struct {
    void *data;
    void *queue[2];
} uv__messenger_item_t;

void uv__messenger_callback(uv_async_t *async) {
    uv_messenger_t *msgr = (uv_messenger_t *)async->data;

    uv__messenger_item_t *item;
    QUEUE *head;

    while (1) {
        uv_mutex_lock(&msgr->mutex);
        if (QUEUE_EMPTY(&msgr->queue)) {
            uv_mutex_unlock(&msgr->mutex);
            break;
        }

        head = QUEUE_HEAD(&msgr->queue);
        item = QUEUE_DATA(head, uv__messenger_item_t, queue);
        QUEUE_REMOVE(head);
        uv_mutex_unlock(&msgr->mutex);

        msgr->callback(item->data);

        free(item);
    }
}

int uv_messenger_init(uv_loop_t *loop, uv_messenger_t *msgr, uv_messenger_cb callback) {
    int ret = uv_mutex_init(&msgr->mutex);
    if (ret < 0) {
        return ret;
    }

    msgr->callback = callback;

    QUEUE_INIT(&msgr->queue);

    msgr->async.data = msgr;
    return uv_async_init(loop, &msgr->async, uv__messenger_callback);
}

void uv_messenger_send(uv_messenger_t *msgr, void *data) {
    uv__messenger_item_t *item = malloc(sizeof(uv__messenger_item_t));
    item->data = data;

    uv_mutex_lock(&msgr->mutex);
    QUEUE_INSERT_TAIL(&msgr->queue, &item->queue);
    uv_mutex_unlock(&msgr->mutex);

    uv_async_send(&msgr->async);
}

void uv_messenger_unref(uv_messenger_t *msgr) {
    uv_unref((uv_handle_t *)&msgr->async);
}

void uv__messenger_stop_callback(uv_handle_t *handle) {
    free((uv_messenger_t *)handle->data);
}

void uv_messenger_stop(uv_messenger_t *msgr) {
    uv_close((uv_handle_t *)&msgr->async, uv__messenger_stop_callback);
}
