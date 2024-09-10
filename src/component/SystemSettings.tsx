import { useState } from "react";

export default function SystemSettings() {
    const [shortcutKey, setShortcutKey] = useState("Ctrl + Alt + S");
    const [action, setAction] = useState("paste");

    return (
        <div>
            <h2 className="text-lg font-bold mb-4">系统设置</h2>

            {/* 快捷键设置 */}
            <div className="mb-6">
                <label className="block mb-2">唤起快捷键设置:</label>
                <input
                    type="text"
                    className="border p-2 rounded w-1/2"
                    value={shortcutKey}
                    onChange={(e) => setShortcutKey(e.target.value)}
                />
            </div>

            {/* 选中短语时行为 */}
            <div className="mb-6">
                <label className="block mb-2">选中短语时的行为:</label>
                <select
                    className="border p-2 rounded w-1/2"
                    value={action}
                    onChange={(e) => setAction(e.target.value)}
                >
                    <option value="paste">粘贴到当前活动窗口</option>
                    <option value="copy">复制到剪贴板</option>
                </select>
            </div>
        </div>
    );
}
