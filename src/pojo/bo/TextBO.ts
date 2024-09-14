export interface TextBO extends Record<string, unknown> {
    id: string | null;
    shortText: string;
    text: string;
}