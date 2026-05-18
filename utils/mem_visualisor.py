#!/usr/bin/env python3
import socket
import threading
import json
import time
import sys

import matplotlib.pyplot as plt
import matplotlib.patches as patches

HOST = '127.0.0.1'
PORT = 9999

latest = None
lock = threading.Lock()


def reader_thread(sock):
    buf = b''
    while True:
        try:
            data = sock.recv(4096)
            if not data:
                break
            buf += data
            while b"\n" in buf:
                line, buf = buf.split(b"\n", 1)
                try:
                    obj = json.loads(line.decode('utf-8'))
                    with lock:
                        global latest
                        latest = obj
                except Exception:
                    pass
        except Exception:
            break


def draw_layout(ax, layout):
    ax.clear()
    mem_total = layout.get('mem_total', 0)
    top = layout.get('top', 0)
    ax.set_xlim(0, mem_total if mem_total>0 else 1)
    ax.set_ylim(0, 1)
    ax.set_yticks([])
    # draw background
    ax.add_patch(patches.Rectangle((0, 0.1), mem_total, 0.8, facecolor='#eee', edgecolor='black'))

    for alloc in layout.get('allocations', []):
        offset = alloc.get('offset', 0)
        size = alloc.get('size', 0)
        locked = alloc.get('locked', 0)
        color = '#8fbf8f' if not locked else '#bf8f8f'
        ax.add_patch(patches.Rectangle((offset, 0.1), size, 0.8, facecolor=color, edgecolor='black'))

    # draw top marker
    ax.plot([top, top], [0, 1], color='red')
    ax.set_title(f"mem_total={mem_total} top={top}")


def main():
    global latest
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        sock.connect((HOST, PORT))
    except Exception as e:
        print('Failed to connect to server:', e)
        sys.exit(1)

    t = threading.Thread(target=reader_thread, args=(sock,), daemon=True)
    t.start()

    fig, ax = plt.subplots(figsize=(8, 2))

    try:
        while True:
            with lock:
                data = latest
            if data:
                draw_layout(ax, data)
                plt.pause(0.1)
            else:
                time.sleep(0.1)
    except KeyboardInterrupt:
        print('Exiting')
    finally:
        sock.close()


if __name__ == '__main__':
    if len(sys.argv) > 1:
        PORT = int(sys.argv[1])
    main()
