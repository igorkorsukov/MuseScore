import argparse
import socket

def sendQuery(q):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect(("localhost", 1222))
        s.sendall(q.encode("utf-8"))
        s.close()

def main():
    parser = argparse.ArgumentParser(description='Send query')
    parser.add_argument('-q', '--query', help='Send query')
    args = parser.parse_args()

    if args.query:
        sendQuery(args.query)
    else:
        parser.print_help()

if __name__ == "__main__":
    main()
