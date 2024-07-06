#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

const int BUFFER_SIZE = 1024;

// 分类文件
std::string getMimeType(const std::string &fileExtension)
{
	if (fileExtension == "html") {
		return "text/html";
	} else if (fileExtension == "css") {
		return "text/css";
	} else if (fileExtension == "js") {
		return "application/javascript";
	} else {
		return "application/octet-stream";
	}
}

// 读取文件
std::string readFile(const std::string &filename)
{
	std::ifstream file(filename, std::ios::in | std::ios::binary);
	if (!file) {
		return "";
	}

	std::ostringstream content;
	content << file.rdbuf();
	return content.str();
}

// 处理客户端请求
void handleRequest(int clientSocket, const std::string &rootDirectory)
{
	// 获取客户端地址信息
	struct sockaddr_in clientAddr;
	socklen_t addrLen = sizeof(clientAddr);
	getpeername(clientSocket, (struct sockaddr *)&clientAddr, &addrLen);
	char clientIP[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
	int clientPort = ntohs(clientAddr.sin_port);

	char buffer[BUFFER_SIZE];
	memset(buffer, 0, sizeof(buffer));
	int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

	if (bytesRead <= 0) {
		close(clientSocket);
		return;
	}

	std::istringstream request(buffer);
	std::string requestLine;
	getline(request, requestLine);
	std::istringstream requestLineStream(requestLine);
	std::string method, path, httpVersion;
	requestLineStream >> method >> path >> httpVersion;

	if (method != "GET") {
		// 输出错误信息
		std::cerr << "Received invalid request from " << clientIP << ":"
			  << clientPort << " - " << requestLine << std::endl;
		close(clientSocket);
		return;
	}

	std::string filename = rootDirectory + path;

	if (filename == rootDirectory + "/") {
		filename = rootDirectory + "/index.html";
	}

	std::string fileExtension =
		filename.substr(filename.find_last_of(".") + 1);
	std::string mimeType = getMimeType(fileExtension);

	std::string fileContent = readFile(filename);
	if (fileContent.empty()) {
		// 文件不存在，尝试读取webroot/error.html
		filename = rootDirectory + "/error.html";
		fileContent = readFile(filename);

		if (fileContent.empty()) {
			// 如果error.html文件也不存在，输出文件未找到信息
			std::cerr << "Requested file not found for " << clientIP
				  << ":" << clientPort << " - " << requestLine
				  << std::endl;
			close(clientSocket);
			return;
		}

		// 如果error.html文件存在，更新MIME类型
		fileExtension = "html";
		mimeType = "text/html";
	}

	// 输出请求来源信息
	std::cout << "Received request from " << clientIP << ":" << clientPort
		  << " - " << requestLine << std::endl;

	// 构建HTTP响应
	std::ostringstream response;
	response << "HTTP/1.1 200 OK\r\n"
		 << "Content-Type: " << mimeType << "\r\n"
		 << "Content-Length: " << fileContent.size() << "\r\n"
		 << "\r\n"
		 << fileContent;

	// 发送HTTP响应
	send(clientSocket, response.str().c_str(), response.str().size(), 0);

	// 输出请求处理完成信息
	std::cout << "Sent response to " << clientIP << ":" << clientPort
		  << " - " << requestLine << std::endl;

	close(clientSocket);
}

int main(int argc, char *argv[])
{
	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " <port> <root_directory>"
			  << std::endl;
		return 1;
	}

	int PORT = std::atoi(argv[1]);
	std::string rootDirectory = argv[2];

	int serverSocket, clientSocket;
	struct sockaddr_in serverAddr, clientAddr;
	socklen_t addrLen = sizeof(clientAddr);

	// 创建套接字
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1) {
		perror("Socket creation failed");
		exit(EXIT_FAILURE);
	}

	// 设置地址重用
	int reuse = 1;
	if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse,
		       sizeof(int)) == -1) {
		perror("Socket option failed");
		exit(EXIT_FAILURE);
	}

	// 配置服务器地址结构
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	// 绑定套接字到地址
	if (bind(serverSocket, (struct sockaddr *)&serverAddr,
		 sizeof(serverAddr)) == -1) {
		perror("Binding failed");
		exit(EXIT_FAILURE);
	}

	// 开始监听客户端连接
	if (listen(serverSocket, 5) == -1) {
		perror("Listening failed");
		exit(EXIT_FAILURE);
	}

	std::cout << "Server is running on port " << PORT
		  << " with root directory " << rootDirectory << std::endl;

	while (1) {
		// 接受客户端连接
		clientSocket = accept(serverSocket,
				      (struct sockaddr *)&clientAddr, &addrLen);
		if (clientSocket == -1) {
			perror("Accepting client connection failed");
			continue;
		}

		// 创建子进程处理客户端请求
		if (fork() == 0) {
			// 子进程
			close(serverSocket); // 关闭父进程的套接字副本
			handleRequest(clientSocket, rootDirectory);
			exit(0);
		}

		close(clientSocket); // 父进程关闭客户端套接字
	}

	close(serverSocket);
	return 0;
}
