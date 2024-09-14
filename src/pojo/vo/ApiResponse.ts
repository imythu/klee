export default class ApiResponse<T> {
    static CODE_SUCCESS = "200";
    static CODE_SERVER_ERROR = "500";

    constructor(
        public code: string,
        public message: string | null,
        public data: T | null) {
    }

    isSuccess(): boolean {
        return this.code === ApiResponse.CODE_SUCCESS;
    }

    static failed<T>(message: string | null): ApiResponse<T>;
    static failed<T>(message: string | null, data: T): ApiResponse<T>;

    // 方法实现
    static failed<T>(message: string | null, data?: T): ApiResponse<T> {
        return new ApiResponse<T>(ApiResponse.CODE_SERVER_ERROR, message, data ?? null);
    }
}