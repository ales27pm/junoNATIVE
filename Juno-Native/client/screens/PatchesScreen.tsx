// ============================================================

import React, { useState, useMemo, useCallback } from "react";
import {
  View,
  StyleSheet,
  FlatList,
  TextInput,
  Pressable,
  Modal,
  Alert,
  Platform,
} from "react-native";
import { useBottomTabBarHeight } from "@react-navigation/bottom-tabs";
import { Feather } from "@expo/vector-icons";
import { ThemedText } from "@/components/ThemedText";
import PatchCard from "@/components/PatchCard";
import { useSynth } from "@/contexts/SynthContext";
import { Colors, Spacing, Typography, BorderRadius } from "@/constants/theme";
import { Patch } from "@/hooks/useSynthEngine";
import { KeyboardAwareScrollViewCompat } from "@/components/KeyboardAwareScrollViewCompat";

const CATEGORIES = ["all", "bass", "lead", "pad", "fx"] as const;

export default function PatchesScreen() {
  const theme = Colors.dark;
  const tabBarHeight = useBottomTabBarHeight();
  const { patches, currentPatchId, loadPatch, savePatch, deletePatch } =
    useSynth();

  const [searchQuery, setSearchQuery] = useState("");
  const [selectedCategory, setSelectedCategory] =
    useState<(typeof CATEGORIES)[number]>("all");
  const [showSaveModal, setShowSaveModal] = useState(false);
  const [newPatchName, setNewPatchName] = useState("");
  const [newPatchCategory, setNewPatchCategory] =
    useState<Patch["category"]>("lead");

  const filteredPatches = useMemo(() => {
    return patches.filter((patch) => {
      const matchesSearch = patch.name
        .toLowerCase()
        .includes(searchQuery.toLowerCase());
      const matchesCategory =
        selectedCategory === "all" || patch.category === selectedCategory;
      return matchesSearch && matchesCategory;
    });
  }, [patches, searchQuery, selectedCategory]);

  const handlePatchPress = useCallback(
    (patch: Patch) => {
      loadPatch(patch);
    },
    [loadPatch],
  );

  const handlePatchLongPress = useCallback(
    (patch: Patch) => {
      if (patch.id.startsWith("factory_")) {
        return;
      }

      if (Platform.OS === "web") {
        if (confirm(`Delete "${patch.name}"?`)) {
          deletePatch(patch.id);
        }
      } else {
        Alert.alert(
          "Delete Patch",
          `Are you sure you want to delete "${patch.name}"?`,
          [
            { text: "Cancel", style: "cancel" },
            {
              text: "Delete",
              style: "destructive",
              onPress: () => deletePatch(patch.id),
            },
          ],
        );
      }
    },
    [deletePatch],
  );

  const handleSavePatch = useCallback(async () => {
    if (!newPatchName.trim()) return;
    await savePatch(newPatchName.trim(), newPatchCategory);
    setNewPatchName("");
    setShowSaveModal(false);
  }, [newPatchName, newPatchCategory, savePatch]);

  const renderPatch = useCallback(
    ({ item }: { item: Patch }) => (
      <View style={styles.patchCardContainer}>
        <PatchCard
          patch={item}
          isActive={item.id === currentPatchId}
          onPress={() => handlePatchPress(item)}
          onLongPress={() => handlePatchLongPress(item)}
        />
      </View>
    ),
    [currentPatchId, handlePatchPress, handlePatchLongPress],
  );

  return (
    <View style={[styles.container, { backgroundColor: theme.backgroundRoot }]}>
      <View style={[styles.searchContainer, { paddingTop: Spacing.lg }]}>
        <View
          style={[
            styles.searchBar,
            { backgroundColor: theme.backgroundSecondary },
          ]}
        >
          <Feather name="search" size={18} color={theme.textSecondary} />
          <TextInput
            style={[styles.searchInput, { color: theme.text }]}
            placeholder="Search patches..."
            placeholderTextColor={theme.textSecondary}
            value={searchQuery}
            onChangeText={setSearchQuery}
          />
        </View>
      </View>

      <View style={styles.categoriesContainer}>
        {CATEGORIES.map((category) => (
          <Pressable
            key={category}
            onPress={() => setSelectedCategory(category)}
            style={[
              styles.categoryChip,
              {
                backgroundColor:
                  selectedCategory === category
                    ? theme.accent
                    : theme.backgroundSecondary,
              },
            ]}
          >
            <ThemedText
              style={[
                styles.categoryText,
                {
                  color:
                    selectedCategory === category
                      ? "#FFFFFF"
                      : theme.textSecondary,
                },
              ]}
            >
              {category.toUpperCase()}
            </ThemedText>
          </Pressable>
        ))}
      </View>

      <FlatList
        data={filteredPatches}
        renderItem={renderPatch}
        keyExtractor={(item) => item.id}
        numColumns={2}
        contentContainerStyle={[
          styles.listContent,
          { paddingBottom: tabBarHeight + Spacing.xl },
        ]}
        columnWrapperStyle={styles.row}
        showsVerticalScrollIndicator={false}
        ListEmptyComponent={
          <View style={styles.emptyContainer}>
            <Feather name="inbox" size={48} color={theme.textSecondary} />
            <ThemedText
              style={[styles.emptyText, { color: theme.textSecondary }]}
            >
              No patches found
            </ThemedText>
          </View>
        }
      />

      <Pressable
        onPress={() => setShowSaveModal(true)}
        style={[
          styles.fab,
          { backgroundColor: theme.accent, bottom: tabBarHeight + Spacing.xl },
        ]}
      >
        <Feather name="plus" size={24} color="#FFFFFF" />
      </Pressable>

      <Modal
        visible={showSaveModal}
        transparent
        animationType="fade"
        onRequestClose={() => setShowSaveModal(false)}
      >
        <View style={styles.modalOverlay}>
          <KeyboardAwareScrollViewCompat
            style={styles.modalScrollView}
            contentContainerStyle={styles.modalScrollContent}
          >
            <View
              style={[
                styles.modalContent,
                { backgroundColor: theme.backgroundDefault },
              ]}
            >
              <ThemedText style={[styles.modalTitle, { color: theme.text }]}>
                Save Patch
              </ThemedText>

              <TextInput
                style={[
                  styles.modalInput,
                  {
                    backgroundColor: theme.backgroundSecondary,
                    color: theme.text,
                    borderColor: theme.divider,
                  },
                ]}
                placeholder="Patch name"
                placeholderTextColor={theme.textSecondary}
                value={newPatchName}
                onChangeText={setNewPatchName}
                autoFocus
              />

              <ThemedText
                style={[styles.modalLabel, { color: theme.textSecondary }]}
              >
                Category
              </ThemedText>
              <View style={styles.categorySelector}>
                {(["bass", "lead", "pad", "fx"] as const).map((cat) => (
                  <Pressable
                    key={cat}
                    onPress={() => setNewPatchCategory(cat)}
                    style={[
                      styles.categorySelectorChip,
                      {
                        backgroundColor:
                          newPatchCategory === cat
                            ? theme.accent
                            : theme.backgroundSecondary,
                      },
                    ]}
                  >
                    <ThemedText
                      style={{
                        color:
                          newPatchCategory === cat
                            ? "#FFFFFF"
                            : theme.textSecondary,
                        ...Typography.knobLabel,
                      }}
                    >
                      {cat.toUpperCase()}
                    </ThemedText>
                  </Pressable>
                ))}
              </View>

              <View style={styles.modalButtons}>
                <Pressable
                  onPress={() => setShowSaveModal(false)}
                  style={[
                    styles.modalButton,
                    { backgroundColor: theme.backgroundSecondary },
                  ]}
                >
                  <ThemedText style={{ color: theme.text }}>Cancel</ThemedText>
                </Pressable>
                <Pressable
                  onPress={handleSavePatch}
                  style={[
                    styles.modalButton,
                    {
                      backgroundColor: theme.accent,
                      opacity: newPatchName.trim() ? 1 : 0.5,
                    },
                  ]}
                >
                  <ThemedText style={{ color: "#FFFFFF" }}>Save</ThemedText>
                </Pressable>
              </View>
            </View>
          </KeyboardAwareScrollViewCompat>
        </View>
      </Modal>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
  },
  searchContainer: {
    paddingHorizontal: Spacing.lg,
    paddingBottom: Spacing.md,
  },
  searchBar: {
    flexDirection: "row",
    alignItems: "center",
    paddingHorizontal: Spacing.md,
    height: 44,
    borderRadius: BorderRadius.xs,
    gap: Spacing.sm,
  },
  searchInput: {
    flex: 1,
    fontSize: 16,
  },
  categoriesContainer: {
    flexDirection: "row",
    paddingHorizontal: Spacing.lg,
    paddingBottom: Spacing.md,
    gap: Spacing.sm,
  },
  categoryChip: {
    paddingHorizontal: Spacing.md,
    paddingVertical: Spacing.sm,
    borderRadius: BorderRadius.xs,
  },
  categoryText: {
    ...Typography.knobLabel,
    fontWeight: "600",
  },
  listContent: {
    paddingHorizontal: Spacing.lg,
    paddingTop: Spacing.sm,
  },
  row: {
    gap: Spacing.md,
  },
  patchCardContainer: {
    flex: 1,
    marginBottom: Spacing.md,
  },
  emptyContainer: {
    alignItems: "center",
    justifyContent: "center",
    paddingVertical: Spacing["5xl"],
    gap: Spacing.md,
  },
  emptyText: {
    ...Typography.body,
  },
  fab: {
    position: "absolute",
    right: Spacing.lg,
    width: 56,
    height: 56,
    borderRadius: 28,
    alignItems: "center",
    justifyContent: "center",
    elevation: 4,
    shadowColor: "#000",
    shadowOffset: { width: 0, height: 2 },
    shadowOpacity: 0.25,
    shadowRadius: 4,
  },
  modalOverlay: {
    flex: 1,
    backgroundColor: "rgba(0, 0, 0, 0.7)",
    justifyContent: "center",
    alignItems: "center",
  },
  modalScrollView: {
    width: "100%",
    maxWidth: 400,
  },
  modalScrollContent: {
    padding: Spacing.lg,
  },
  modalContent: {
    padding: Spacing.xl,
    borderRadius: BorderRadius.md,
    gap: Spacing.lg,
  },
  modalTitle: {
    ...Typography.h4,
    textAlign: "center",
  },
  modalInput: {
    height: 48,
    borderRadius: BorderRadius.xs,
    paddingHorizontal: Spacing.md,
    fontSize: 16,
    borderWidth: 1,
  },
  modalLabel: {
    ...Typography.small,
  },
  categorySelector: {
    flexDirection: "row",
    gap: Spacing.sm,
  },
  categorySelectorChip: {
    flex: 1,
    paddingVertical: Spacing.sm,
    alignItems: "center",
    borderRadius: BorderRadius.xs,
  },
  modalButtons: {
    flexDirection: "row",
    gap: Spacing.md,
    marginTop: Spacing.md,
  },
  modalButton: {
    flex: 1,
    height: 48,
    borderRadius: BorderRadius.xs,
    alignItems: "center",
    justifyContent: "center",
  },
});

// ============================================================
