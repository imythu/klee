import { useState } from "react";
import {TauriClient} from "../tauri/TauriClient.ts";

interface TextItem {
    id: number;
    shortcut: string;
    content: string;
}

const initialTexts: TextItem[] = [
    { id: 1, shortcut: "greet", content: "Hello, how are you?" },
    { id: 2, shortcut: "bye", content: "Goodbye!" },
    // 可以继续添加示例数据
];

export default function TextConfig() {
    const [texts, setTexts] = useState(initialTexts);
    const [search, setSearch] = useState("");
    const [isAddModalOpen, setIsAddModalOpen] = useState(false);
    const [isEditModalOpen, setIsEditModalOpen] = useState(false);
    const [, setEditText] = useState<TextItem | null>(null);

    // 处理搜索
    const filteredTexts = texts.filter((text) =>
        text.shortcut.includes(search) || text.content.includes(search)
    );

    // 删除文本
    const handleDelete = (id: number) => {
        if (window.confirm("确定要删除吗？")) {
            setTexts(texts.filter((text) => text.id !== id));
        }
    };

    // 打开编辑模态框
    const handleEdit = (text: TextItem) => {
        setEditText(text);
        setIsEditModalOpen(true);
    };

    return (
        <div>
            {/* 搜索框 */}
            <div className="flex justify-between items-center mb-4">
                <input
                    type="text"
                    placeholder="搜索..."
                    className="border p-2 rounded w-1/3"
                    value={search}
                    onChange={(e) => setSearch(e.target.value)}
                    onKeyDown={(e) => {
                        if (e.key === "Enter") {
                            e.preventDefault();
                            // 执行搜索逻辑
                            TauriClient.pasteIntoCurrentWindow(search)
                        }
                    }}
                />
                <button
                    className="bg-blue-500 text-white p-2 rounded"
                    onClick={() => setIsAddModalOpen(true)}
                >
                    新增文本
                </button>
            </div>

            {/* 表格 */}
            <table className="w-full text-left border-collapse">
                <thead>
                <tr>
                    <th className="border p-2">快捷短语</th>
                    <th className="border p-2">文本内容</th>
                    <th className="border p-2">操作</th>
                </tr>
                </thead>
                <tbody>
                {filteredTexts.map((text) => (
                    <tr key={text.id}>
                        <td className="border p-2">{text.shortcut}</td>
                        <td className="border p-2">{text.content}</td>
                        <td className="border p-2">
                            <button
                                className="bg-yellow-500 text-white p-1 rounded mr-2"
                                onClick={() => handleEdit(text)}
                            >
                                修改
                            </button>
                            <button
                                className="bg-red-500 text-white p-1 rounded"
                                onClick={() => handleDelete(text.id)}
                            >
                                删除
                            </button>
                        </td>
                    </tr>
                ))}
                </tbody>
            </table>

            {/* 新增模态框 */}
            {isAddModalOpen && (
                <div className="fixed inset-0 bg-black bg-opacity-50 flex justify-center items-center">
                    <div className="bg-white p-4 rounded shadow">
                        <h2 className="text-lg mb-4">新增文本</h2>
                        {/* 这里可以添加输入字段和保存逻辑 */}
                        <button
                            className="bg-blue-500 text-white p-2 rounded"
                            onClick={() => setIsAddModalOpen(false)}
                        >
                            关闭
                        </button>
                    </div>
                </div>
            )}

            {/* 编辑模态框 */}
            {isEditModalOpen && (
                <div className="fixed inset-0 bg-black bg-opacity-50 flex justify-center items-center">
                    <div className="bg-white p-4 rounded shadow">
                        <h2 className="text-lg mb-4">编辑文本</h2>
                        {/* 这里可以添加编辑字段和保存逻辑 */}
                        <button
                            className="bg-blue-500 text-white p-2 rounded"
                            onClick={() => setIsEditModalOpen(false)}
                        >
                            关闭
                        </button>
                    </div>
                </div>
            )}
        </div>
    );
}
