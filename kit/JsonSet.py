#!/usr/bin/env python3
"""
JSON Branch Modifier Tool
Adds main branches and sub-branches to JSON files with conflict resolution
"""

import json
import sys
from typing import Union

def add_json_branches(file_path: str, main_branch: str, *sub_branches: str) -> bool:
    """
    Modifies JSON file by adding branches with conflict handling
    
    Args:
        file_path: Path to JSON file
        main_branch: Name of main branch to add/modify
        sub_branches: Sub-branches in format "key=value" or "key"
    
    Returns:
        bool: True if successful, False otherwise
    """
    # Parse force mode flag
    force_mode = '--force' in sub_branches
    sub_branches = [arg for arg in sub_branches if arg != '--force']
    
    try:
        # Load JSON data
        with open(file_path, 'r') as f:
            data = json.load(f)
        
        # Main branch handling
        if main_branch in data:
            print(f"[INFO] Main branch '{main_branch}' exists - modifying")
            branch_data = data[main_branch]
        else:
            branch_data = {}
            data[main_branch] = branch_data
            print(f"[INFO] Created new main branch: '{main_branch}'")
        
        # Process sub-branches
        added = updated = skipped = 0
        for sub in sub_branches:
            # Parse key-value pair
            if '=' in sub:
                key, value = sub.split('=', 1)
                # Convert numeric values
                try:
                    value = int(value) if value.isdigit() else float(value)
                except ValueError:
                    pass  # Keep as string
            else:
                key, value = sub, None  # Default None value
            
            # Conflict resolution
            if key in branch_data:
                current_val = branch_data[key]
                if current_val != value:
                    if force_mode:
                        branch_data[key] = value
                        updated += 1
                        print(f"[UPDATE] Forced update: '{key}' = '{value}' (was: '{current_val}')")
                    else:
                        choice = input(
                            f"[CONFLICT] '{key}' exists (current: '{current_val}'). "
                            f"Overwrite with '{value}'? [y/N] "
                        ).lower()
                        if choice == 'y':
                            branch_data[key] = value
                            updated += 1
                            print(f"[UPDATE] Updated '{key}'")
                        else:
                            skipped += 1
                            print(f"[SKIP] Kept original value for '{key}'")
                else:
                    skipped += 1
                    print(f"[SKIP] Value unchanged for '{key}'")
            else:
                branch_data[key] = value
                added += 1
                print(f"[ADD] New sub-branch: '{key}' = '{value}'")
        
        # Save modified data
        with open(file_path, 'w') as f:
            json.dump(data, f, indent=2, ensure_ascii=False)
        
        print(
            f"[SUCCESS] Operations completed: "
            f"{added} added, {updated} updated, {skipped} skipped"
        )
        return True
        
    except FileNotFoundError:
        print("[ERROR] File not found:", file_path)
        return False
    except json.JSONDecodeError:
        print("[ERROR] Invalid JSON format in", file_path)
        return False
    except Exception as e:
        print(f"[ERROR] {str(e)}")
        return False

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print(
            "Usage: ./json_branch_modifier.py <file> <main_branch> [--force] "
            "[sub1=val1] [sub2] ...\n"
            "Examples:\n"
            "  ./json_branch_modifier.py config.json NewBranch param1=100 param2\n"
            "  ./json_branch_modifier.py config.json ExistingBranch --force param1=new_value"
        )
        sys.exit(1)
    
    success = add_json_branches(*sys.argv[1:])
    sys.exit(0 if success else 1)
