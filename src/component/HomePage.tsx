import Menu from "./Menu.tsx";
import TextConfig from "./TextConfig.tsx";
import SystemSettings from "./SystemSettings.tsx";
import {useState} from "react";

export default function HomePage() {
    const [currentTab, setCurrentTab] = useState("textConfig");
    return (
        <div className="flex h-screen bg-gray-50">
            {/* 左侧菜单 */}
            <div className="w-1/5 border-r border-gray-200 p-6">
                <Menu currentTab={currentTab} setCurrentTab={setCurrentTab}/>
            </div>

            {/* 右侧内容区域 */}
            <div className="flex-1 p-8">
                <div className="bg-white shadow rounded-lg p-6">
                    {currentTab === "textConfig" && <TextConfig/>}
                    {currentTab === "systemSettings" && <SystemSettings/>}
                </div>
            </div>
        </div>
    );
}