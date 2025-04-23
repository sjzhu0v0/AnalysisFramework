#!/usr/bin/env python3
"""
JSON Branch Modifier Tool
Now with proper backslash handling in values and paths
"""

import json
import sys
import re
from typing import Union

def unescape_backslashes(value: str) -> str:
    """Handle escaped backslashes in input strings"""
    return value.replace('\\\\', '\x00').replace('\\', '').replace('\x00', '\\')

def escape_backslashes(value: str) -> str:
    """Escape backslashes for JSON output"""
    return value.replace('\\', '\\\\')

def get_nested_value(data: dict, path: str) -> Union[dict, list, str, int, float, None]:
    """Get a value from nested dictionary using dot notation path"""
    keys = path.split('.')
    current = data
    for key in keys:
        if isinstance(current, dict) and key in current:
            current = current[key]
        else:
            raise KeyError(f"Path '{path}' not found in JSON structure")
    return current

def add_json_branches(file_path: str, main_branch: str, *sub_branches: str) -> bool:
    """
    Modifies JSON file by adding branches with conflict handling
    Supports copying values from other branches using source:key syntax
    Handles backslashes in paths and values
    
    Args:
        file_path: Path to JSON file
        main_branch: Name of main branch to add/modify
        sub_branches: Sub-branches in format:
                      - "key=value" for direct values
                      - "key=source_branch:source_key" for copying
                      - "key" for None value
    
    Returns:
        bool: True if successful, False otherwise
    """
    # Parse flags
    force_mode = '--force' in sub_branches
    mute_mode = '--mute' in sub_branches
    sub_branches = [arg for arg in sub_branches if arg not in ('--force', '--mute')]
    
    try:
        # Load JSON data
        with open(file_path, 'r') as f:
            data = json.load(f)
        
        # Create main branch if it doesn't exist
        if main_branch not in data:
            data[main_branch] = {}
            if not mute_mode:
                print(f"[INFO] Created new main branch: '{main_branch}'")
        
        branch_data = data[main_branch]
        
        # Process sub-branches
        added = updated = skipped = 0
        for sub in sub_branches:
            # Parse key-value pair
            if '=' in sub:
                key, value = sub.split('=', 1)
                
                # Handle backslashes in the key
                key = unescape_backslashes(key)
                
                # Handle copy from other branch (source_branch:source_key)
                if ':' in value:
                    source_branch, source_key = value.split(':', 1)
                    source_branch = unescape_backslashes(source_branch)
                    source_key = unescape_backslashes(source_key)
                    try:
                        if source_branch not in data:
                            raise KeyError(f"Source branch '{source_branch}' not found")
                        value = get_nested_value(data[source_branch], source_key)
                        if not mute_mode:
                            print(f"[COPY] Copied value from '{source_branch}.{source_key}'")
                    except Exception as e:
                        if not mute_mode:
                            print(f"[ERROR] Failed to copy value: {str(e)}")
                        continue
                else:
                    # Handle backslashes in direct values
                    value = unescape_backslashes(value)
                    # Convert numeric values
                    try:
                        value = int(value) if value.isdigit() else float(value)
                    except ValueError:
                        pass  # Keep as string
            else:
                key, value = unescape_backslashes(sub), None  # Default None value
            
            # Escape backslashes before JSON storage
            if isinstance(value, str):
                value = escape_backslashes(value)
            
            # Conflict resolution
            if key in branch_data:
                current_val = branch_data[key]
                if current_val != value:
                    if force_mode:
                        branch_data[key] = value
                        updated += 1
                        if not mute_mode:
                            print(f"[UPDATE] Forced update: '{key}' = '{value}' (was: '{current_val}')")
                    else:
                        if mute_mode:
                            # In mute mode, skip conflicts unless forced
                            skipped += 1
                            continue
                        choice = input(
                            f"[CONFLICT] '{key}' exists (current: '{current_val}'). "
                            f"Overwrite with '{value}'? [y/N] "
                        ).lower()
                        if choice == 'y':
                            branch_data[key] = value
                            updated += 1
                            if not mute_mode:
                                print(f"[UPDATE] Updated '{key}'")
                        else:
                            skipped += 1
                            if not mute_mode:
                                print(f"[SKIP] Kept original value for '{key}'")
                else:
                    skipped += 1
                    if not mute_mode:
                        print(f"[SKIP] Value unchanged for '{key}'")
            else:
                branch_data[key] = value
                added += 1
                if not mute_mode:
                    print(f"[ADD] New sub-branch: '{key}' = '{value}'")
        
        # Save modified data with proper backslash escaping
        with open(file_path, 'w') as f:
            json.dump(data, f, indent=2, ensure_ascii=False)
        
        if not mute_mode:
            print(
                f"[SUCCESS] Operations completed: "
                f"{added} added, {updated} updated, {skipped} skipped"
            )
        return True
        
    except FileNotFoundError:
        if not mute_mode:
            print("[ERROR] File not found:", file_path)
        return False
    except json.JSONDecodeError:
        if not mute_mode:
            print("[ERROR] Invalid JSON format in", file_path)
        return False
    except Exception as e:
        if not mute_mode:
            print(f"[ERROR] {str(e)}")
        return False

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print(
            "Usage: ./json_branch_modifier.py <file> <main_branch> [--force] [--mute] "
            "[sub1=val1] [sub2=source_branch:source_key] [sub3] ...\n"
            "Note: Use \\\\ to represent a single backslash in values\n"
            "Examples:\n"
            "  ./json_branch_modifier.py config.json NewBranch path=C:\\\\temp\n"
            "  ./json_branch_modifier.py config.json ME_PR_thn path_input=SE_PR_thn:path_input\n"
            "  ./json_branch_modifier.py config.json Paths --force win_path=C:\\\\Program Files\\\\App\n"
            "  ./json_branch_modifier.py config.json Paths --mute silent_path=C:\\\\Silent"
        )
        sys.exit(1)
    
    success = add_json_branches(*sys.argv[1:])
    sys.exit(0 if success else 1)