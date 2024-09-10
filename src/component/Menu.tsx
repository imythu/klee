interface MenuProps {
    currentTab: string;
    setCurrentTab: (tab: string) => void;
}

export default function Menu({ currentTab, setCurrentTab }: MenuProps) {
    return (
        <div className="flex flex-col space-y-4">
            <button
                className={`${
                    currentTab === "textConfig" ? "bg-blue-500 text-white" : "bg-gray-100"
                } p-2 rounded`}
                onClick={() => setCurrentTab("textConfig")}
            >
                文本配置
            </button>
            <button
                className={`${
                    currentTab === "systemSettings" ? "bg-blue-500 text-white" : "bg-gray-100"
                } p-2 rounded`}
                onClick={() => setCurrentTab("systemSettings")}
            >
                系统设置
            </button>
        </div>
    );
}
